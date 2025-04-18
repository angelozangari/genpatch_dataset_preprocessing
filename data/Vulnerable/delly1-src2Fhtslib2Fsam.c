/*  sam.c -- SAM and BAM file I/O and manipulation.

    Copyright (C) 2008-2010, 2012-2017 Genome Research Ltd.
    Copyright (C) 2010, 2012, 2013 Broad Institute.

    Author: Heng Li <lh3@sanger.ac.uk>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.  */

#include <config.h>

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <zlib.h>
#include <assert.h>
#include "htslib/sam.h"
#include "htslib/bgzf.h"
#include "cram/cram.h"
#include "hts_internal.h"
#include "htslib/hfile.h"
#include "htslib/hts_endian.h"

#include "htslib/khash.h"
KHASH_DECLARE(s2i, kh_cstr_t, int64_t)

typedef khash_t(s2i) sdict_t;

#ifndef EOVERFLOW
#define EOVERFLOW ERANGE
#endif

/**********************
 *** BAM header I/O ***
 **********************/

bam_hdr_t *bam_hdr_init()
{
    return (bam_hdr_t*)calloc(1, sizeof(bam_hdr_t));
}

void bam_hdr_destroy(bam_hdr_t *h)
{
    int32_t i;
    if (h == NULL) return;
    if (h->target_name) {
        for (i = 0; i < h->n_targets; ++i)
            free(h->target_name[i]);
        free(h->target_name);
        free(h->target_len);
    }
    free(h->text); free(h->cigar_tab);
    if (h->sdict) kh_destroy(s2i, (sdict_t*)h->sdict);
    free(h);
}

bam_hdr_t *bam_hdr_dup(const bam_hdr_t *h0)
{
    if (h0 == NULL) return NULL;
    bam_hdr_t *h;
    if ((h = bam_hdr_init()) == NULL) return NULL;
    // copy the simple data
    h->n_targets = h0->n_targets;
    h->ignore_sam_err = h0->ignore_sam_err;
    h->l_text = h0->l_text;
    // Then the pointery stuff
    h->cigar_tab = NULL;
    h->sdict = NULL;
    // TODO Check for memory allocation failures
    h->text = (char*)calloc(h->l_text + 1, 1);
    memcpy(h->text, h0->text, h->l_text);
    h->target_len = (uint32_t*)calloc(h->n_targets, sizeof(uint32_t));
    h->target_name = (char**)calloc(h->n_targets, sizeof(char*));
    int i;
    for (i = 0; i < h->n_targets; ++i) {
        h->target_len[i] = h0->target_len[i];
        h->target_name[i] = strdup(h0->target_name[i]);
    }
    return h;
}


static bam_hdr_t *hdr_from_dict(sdict_t *d)
{
    bam_hdr_t *h;
    khint_t k;
    h = bam_hdr_init();
    h->sdict = d;
    h->n_targets = kh_size(d);
    // TODO Check for memory allocation failures
    h->target_len = (uint32_t*)malloc(sizeof(uint32_t) * h->n_targets);
    h->target_name = (char**)malloc(sizeof(char*) * h->n_targets);
    for (k = kh_begin(d); k != kh_end(d); ++k) {
        if (!kh_exist(d, k)) continue;
        h->target_name[kh_val(d, k)>>32] = (char*)kh_key(d, k);
        h->target_len[kh_val(d, k)>>32]  = kh_val(d, k) & 0xffffffffUL;
        kh_val(d, k) >>= 32;
    }
    return h;
}

bam_hdr_t *bam_hdr_read(BGZF *fp)
{
    bam_hdr_t *h;
    char buf[4];
    int magic_len, has_EOF;
    int32_t i, name_len, num_names = 0;
    size_t bufsize;
    ssize_t bytes;
    // check EOF
    has_EOF = bgzf_check_EOF(fp);
    if (has_EOF < 0) {
        perror("[W::bam_hdr_read] bgzf_check_EOF");
    } else if (has_EOF == 0) {
        hts_log_warning("EOF marker is absent. The input is probably truncated");
    }
    // read "BAM1"
    magic_len = bgzf_read(fp, buf, 4);
    if (magic_len != 4 || strncmp(buf, "BAM\1", 4)) {
        hts_log_error("Invalid BAM binary header");
        return 0;
    }
    h = bam_hdr_init();
    if (!h) goto nomem;

    // read plain text and the number of reference sequences
    bytes = bgzf_read(fp, &h->l_text, 4);
    if (bytes != 4) goto read_err;
    if (fp->is_be) ed_swap_4p(&h->l_text);

    bufsize = ((size_t) h->l_text) + 1;
    if (bufsize < h->l_text) goto nomem; // so large that adding 1 overflowed
    h->text = (char*)malloc(bufsize);
    if (!h->text) goto nomem;
    h->text[h->l_text] = 0; // make sure it is NULL terminated
    bytes = bgzf_read(fp, h->text, h->l_text);
    if (bytes != h->l_text) goto read_err;

    bytes = bgzf_read(fp, &h->n_targets, 4);
    if (bytes != 4) goto read_err;
    if (fp->is_be) ed_swap_4p(&h->n_targets);

    if (h->n_targets < 0) goto invalid;

    // read reference sequence names and lengths
    if (h->n_targets > 0) {
        h->target_name = (char**)calloc(h->n_targets, sizeof(char*));
        if (!h->target_name) goto nomem;
        h->target_len = (uint32_t*)calloc(h->n_targets, sizeof(uint32_t));
        if (!h->target_len) goto nomem;
    }
    else {
        h->target_name = NULL;
        h->target_len = NULL;
    }

    for (i = 0; i != h->n_targets; ++i) {
        bytes = bgzf_read(fp, &name_len, 4);
        if (bytes != 4) goto read_err;
        if (fp->is_be) ed_swap_4p(&name_len);
        if (name_len <= 0) goto invalid;

        h->target_name[i] = (char*)malloc(name_len);
        if (!h->target_name[i]) goto nomem;
        num_names++;

        bytes = bgzf_read(fp, h->target_name[i], name_len);
        if (bytes != name_len) goto read_err;

        if (h->target_name[i][name_len - 1] != '\0') {
            /* Fix missing NUL-termination.  Is this being too nice?
               We could alternatively bail out with an error. */
            char *new_name;
            if (name_len == INT32_MAX) goto invalid;
            new_name = realloc(h->target_name[i], name_len + 1);
            if (new_name == NULL) goto nomem;
            h->target_name[i] = new_name;
            h->target_name[i][name_len] = '\0';
        }

        bytes = bgzf_read(fp, &h->target_len[i], 4);
        if (bytes != 4) goto read_err;
        if (fp->is_be) ed_swap_4p(&h->target_len[i]);
    }
    return h;

 nomem:
    hts_log_error("Out of memory");
    goto clean;

 read_err:
    if (bytes < 0) {
        hts_log_error("Error reading BGZF stream");
    } else {
        hts_log_error("Truncated BAM header");
    }
    goto clean;

 invalid:
    hts_log_error("Invalid BAM binary header");

 clean:
    if (h != NULL) {
        h->n_targets = num_names; // ensure we free only allocated target_names
        bam_hdr_destroy(h);
    }
    return NULL;
}

int bam_hdr_write(BGZF *fp, const bam_hdr_t *h)
{
    char buf[4];
    int32_t i, name_len, x;
    // write "BAM1"
    strncpy(buf, "BAM\1", 4);
    if (bgzf_write(fp, buf, 4) < 0) return -1;
    // write plain text and the number of reference sequences
    if (fp->is_be) {
        x = ed_swap_4(h->l_text);
        if (bgzf_write(fp, &x, 4) < 0) return -1;
        if (h->l_text) {
            if (bgzf_write(fp, h->text, h->l_text) < 0) return -1;
        }
        x = ed_swap_4(h->n_targets);
        if (bgzf_write(fp, &x, 4) < 0) return -1;
    } else {
        if (bgzf_write(fp, &h->l_text, 4) < 0) return -1;
        if (h->l_text) {
            if (bgzf_write(fp, h->text, h->l_text) < 0) return -1;
        }
        if (bgzf_write(fp, &h->n_targets, 4) < 0) return -1;
    }
    // write sequence names and lengths
    for (i = 0; i != h->n_targets; ++i) {
        char *p = h->target_name[i];
        name_len = strlen(p) + 1;
        if (fp->is_be) {
            x = ed_swap_4(name_len);
            if (bgzf_write(fp, &x, 4) < 0) return -1;
        } else {
            if (bgzf_write(fp, &name_len, 4) < 0) return -1;
        }
        if (bgzf_write(fp, p, name_len) < 0) return -1;
        if (fp->is_be) {
            x = ed_swap_4(h->target_len[i]);
            if (bgzf_write(fp, &x, 4) < 0) return -1;
        } else {
            if (bgzf_write(fp, &h->target_len[i], 4) < 0) return -1;
        }
    }
    if (bgzf_flush(fp) < 0) return -1;
    return 0;
}

int bam_name2id(bam_hdr_t *h, const char *ref)
{
    sdict_t *d = (sdict_t*)h->sdict;
    khint_t k;
    if (h->sdict == 0) {
        int i, absent;
        d = kh_init(s2i);
        for (i = 0; i < h->n_targets; ++i) {
            k = kh_put(s2i, d, h->target_name[i], &absent);
            kh_val(d, k) = i;
        }
        h->sdict = d;
    }
    k = kh_get(s2i, d, ref);
    return k == kh_end(d)? -1 : kh_val(d, k);
}

/*************************
 *** BAM alignment I/O ***
 *************************/

bam1_t *bam_init1()
{
    return (bam1_t*)calloc(1, sizeof(bam1_t));
}

void bam_destroy1(bam1_t *b)
{
    if (b == 0) return;
    free(b->data); free(b);
}

bam1_t *bam_copy1(bam1_t *bdst, const bam1_t *bsrc)
{
    uint8_t *data = bdst->data;
    int m_data = bdst->m_data;   // backup data and m_data
    if (m_data < bsrc->l_data) { // double the capacity
        m_data = bsrc->l_data; kroundup32(m_data);
        data = (uint8_t*)realloc(data, m_data);
    }
    memcpy(data, bsrc->data, bsrc->l_data); // copy var-len data
    *bdst = *bsrc; // copy the rest
    // restore the backup
    bdst->m_data = m_data;
    bdst->data = data;
    return bdst;
}

bam1_t *bam_dup1(const bam1_t *bsrc)
{
    if (bsrc == NULL) return NULL;
    bam1_t *bdst = bam_init1();
    if (bdst == NULL) return NULL;
    return bam_copy1(bdst, bsrc);
}

int bam_cigar2qlen(int n_cigar, const uint32_t *cigar)
{
    int k, l;
    for (k = l = 0; k < n_cigar; ++k)
        if (bam_cigar_type(bam_cigar_op(cigar[k]))&1)
            l += bam_cigar_oplen(cigar[k]);
    return l;
}

int bam_cigar2rlen(int n_cigar, const uint32_t *cigar)
{
    int k, l;
    for (k = l = 0; k < n_cigar; ++k)
        if (bam_cigar_type(bam_cigar_op(cigar[k]))&2)
            l += bam_cigar_oplen(cigar[k]);
    return l;
}

int32_t bam_endpos(const bam1_t *b)
{
    if (!(b->core.flag & BAM_FUNMAP) && b->core.n_cigar > 0)
        return b->core.pos + bam_cigar2rlen(b->core.n_cigar, bam_get_cigar(b));
    else
        return b->core.pos + 1;
}

static inline int aux_type2size(uint8_t type)
{
    switch (type) {
    case 'A': case 'c': case 'C':
        return 1;
    case 's': case 'S':
        return 2;
    case 'i': case 'I': case 'f':
        return 4;
    case 'd':
        return 8;
    case 'Z': case 'H': case 'B':
        return type;
    default:
        return 0;
    }
}

static void swap_data(const bam1_core_t *c, int l_data, uint8_t *data, int is_host)
{
    uint32_t *cigar = (uint32_t*)(data + c->l_qname);
    uint32_t i;
    for (i = 0; i < c->n_cigar; ++i) ed_swap_4p(&cigar[i]);
}

int bam_read1(BGZF *fp, bam1_t *b)
{
    bam1_core_t *c = &b->core;
    int32_t block_len, ret, i;
    uint32_t x[8];
    if ((ret = bgzf_read(fp, &block_len, 4)) != 4) {
        if (ret == 0) return -1; // normal end-of-file
        else return -2; // truncated
    }
    if (fp->is_be)
        ed_swap_4p(&block_len);
    if (block_len < 32) return -4;  // block_len includes core data
    if (bgzf_read(fp, x, 32) != 32) return -3;
    if (fp->is_be) {
        for (i = 0; i < 8; ++i) ed_swap_4p(x + i);
    }
    c->tid = x[0]; c->pos = x[1];
    c->bin = x[2]>>16; c->qual = x[2]>>8&0xff; c->l_qname = x[2]&0xff;
    c->l_extranul = (c->l_qname%4 != 0)? (4 - c->l_qname%4) : 0;
    if ((uint32_t) c->l_qname + c->l_extranul > 255) // l_qname would overflow
        return -4;
    c->flag = x[3]>>16; c->n_cigar = x[3]&0xffff;
    c->l_qseq = x[4];
    c->mtid = x[5]; c->mpos = x[6]; c->isize = x[7];
    b->l_data = block_len - 32 + c->l_extranul;
    if (b->l_data < 0 || c->l_qseq < 0 || c->l_qname < 1) return -4;
    if (((uint64_t) c->n_cigar << 2) + c->l_qname + c->l_extranul
        + (((uint64_t) c->l_qseq + 1) >> 1) + c->l_qseq > (uint64_t) b->l_data)
        return -4;
    if (b->m_data < b->l_data) {
        uint8_t *new_data;
        uint32_t new_m = b->l_data;
        kroundup32(new_m);
        new_data = (uint8_t*)realloc(b->data, new_m);
        if (!new_data)
            return -4;
        b->data = new_data;
        b->m_data = new_m;
    }
    if (bgzf_read(fp, b->data, c->l_qname) != c->l_qname) return -4;
    for (i = 0; i < c->l_extranul; ++i) b->data[c->l_qname+i] = '\0';
    c->l_qname += c->l_extranul;
    if (b->l_data < c->l_qname ||
        bgzf_read(fp, b->data + c->l_qname, b->l_data - c->l_qname) != b->l_data - c->l_qname)
        return -4;
    if (fp->is_be) swap_data(c, b->l_data, b->data, 0);

    // Sanity check for broken CIGAR alignments
    if (c->n_cigar > 0 && c->l_qseq > 0 && !(c->flag & BAM_FUNMAP)
        && bam_cigar2qlen(c->n_cigar, bam_get_cigar(b)) != c->l_qseq) {
        hts_log_error("CIGAR and query sequence lengths differ for %s",
                      bam_get_qname(b));
        return -4;
    }

    return 4 + block_len;
}

int bam_write1(BGZF *fp, const bam1_t *b)
{
    const bam1_core_t *c = &b->core;
    uint32_t x[8], block_len = b->l_data - c->l_extranul + 32, y;
    int i, ok;
    if (c->n_cigar >= 65536) {
        hts_log_error("Too many CIGAR operations (%d >= 64K for QNAME \"%s\")", c->n_cigar, bam_get_qname(b));
        errno = EOVERFLOW;
        return -1;
    }
    x[0] = c->tid;
    x[1] = c->pos;
    x[2] = (uint32_t)c->bin<<16 | c->qual<<8 | (c->l_qname - c->l_extranul);
    x[3] = (uint32_t)c->flag<<16 | c->n_cigar;
    x[4] = c->l_qseq;
    x[5] = c->mtid;
    x[6] = c->mpos;
    x[7] = c->isize;
    ok = (bgzf_flush_try(fp, 4 + block_len) >= 0);
    if (fp->is_be) {
        for (i = 0; i < 8; ++i) ed_swap_4p(x + i);
        y = block_len;
        if (ok) ok = (bgzf_write(fp, ed_swap_4p(&y), 4) >= 0);
        swap_data(c, b->l_data, b->data, 1);
    } else {
        if (ok) ok = (bgzf_write(fp, &block_len, 4) >= 0);
    }
    if (ok) ok = (bgzf_write(fp, x, 32) >= 0);
    if (ok) ok = (bgzf_write(fp, b->data, c->l_qname - c->l_extranul) >= 0);
    if (ok) ok = (bgzf_write(fp, b->data + c->l_qname, b->l_data - c->l_qname) >= 0);
    if (fp->is_be) swap_data(c, b->l_data, b->data, 0);
    return ok? 4 + block_len : -1;
}

/********************
 *** BAM indexing ***
 ********************/

static hts_idx_t *bam_index(BGZF *fp, int min_shift)
{
    int n_lvls, i, fmt, ret;
    bam1_t *b;
    hts_idx_t *idx;
    bam_hdr_t *h;
    h = bam_hdr_read(fp);
    if (h == NULL) return NULL;
    if (min_shift > 0) {
        int64_t max_len = 0, s;
        for (i = 0; i < h->n_targets; ++i)
            if (max_len < h->target_len[i]) max_len = h->target_len[i];
        max_len += 256;
        for (n_lvls = 0, s = 1<<min_shift; max_len > s; ++n_lvls, s <<= 3);
        fmt = HTS_FMT_CSI;
    } else min_shift = 14, n_lvls = 5, fmt = HTS_FMT_BAI;
    idx = hts_idx_init(h->n_targets, fmt, bgzf_tell(fp), min_shift, n_lvls);
    bam_hdr_destroy(h);
    b = bam_init1();
    while ((ret = bam_read1(fp, b)) >= 0) {
        ret = hts_idx_push(idx, b->core.tid, b->core.pos, bam_endpos(b), bgzf_tell(fp), !(b->core.flag&BAM_FUNMAP));
        if (ret < 0) goto err; // unsorted
    }
    if (ret < -1) goto err; // corrupted BAM file

    hts_idx_finish(idx, bgzf_tell(fp));
    bam_destroy1(b);
    return idx;

err:
    bam_destroy1(b);
    hts_idx_destroy(idx);
    return NULL;
}

int sam_index_build3(const char *fn, const char *fnidx, int min_shift, int nthreads)
{
    hts_idx_t *idx;
    htsFile *fp;
    int ret = 0;

    if ((fp = hts_open(fn, "r")) == 0) return -2;
    if (nthreads)
        hts_set_threads(fp, nthreads);

    switch (fp->format.format) {
    case cram:
        ret = cram_index_build(fp->fp.cram, fn, fnidx);
        break;

    case bam:
        idx = bam_index(fp->fp.bgzf, min_shift);
        if (idx) {
            ret = hts_idx_save_as(idx, fn, fnidx, (min_shift > 0)? HTS_FMT_CSI : HTS_FMT_BAI);
            if (ret < 0) ret = -4;
            hts_idx_destroy(idx);
        }
        else ret = -1;
        break;

    default:
        ret = -3;
        break;
    }
    hts_close(fp);

    return ret;
}

int sam_index_build2(const char *fn, const char *fnidx, int min_shift)
{
    return sam_index_build3(fn, fnidx, min_shift, 0);
}

int sam_index_build(const char *fn, int min_shift)
{
    return sam_index_build3(fn, NULL, min_shift, 0);
}

// Provide bam_index_build() symbol for binary compability with earlier HTSlib
#undef bam_index_build
int bam_index_build(const char *fn, int min_shift)
{
    return sam_index_build2(fn, NULL, min_shift);
}

static int bam_readrec(BGZF *fp, void *ignored, void *bv, int *tid, int *beg, int *end)
{
    bam1_t *b = bv;
    int ret;
    if ((ret = bam_read1(fp, b)) >= 0) {
        *tid = b->core.tid;
        *beg = b->core.pos;
        *end = bam_endpos(b);
    }
    return ret;
}

// This is used only with read_rest=1 iterators, so need not set tid/beg/end.
static int cram_readrec(BGZF *ignored, void *fpv, void *bv, int *tid, int *beg, int *end)
{
    htsFile *fp = fpv;
    bam1_t *b = bv;
    int ret = cram_get_bam_seq(fp->fp.cram, &b);
    return ret >= 0
        ? ret
        : (cram_eof(fp->fp.cram) ? -1 : -2);
}

// This is used only with read_rest=1 iterators, so need not set tid/beg/end.
static int sam_bam_cram_readrec(BGZF *bgzfp, void *fpv, void *bv, int *tid, int *beg, int *end)
{
    htsFile *fp = fpv;
    bam1_t *b = bv;
    switch (fp->format.format) {
    case bam:   return bam_read1(bgzfp, b);
    case cram: {
        int ret = cram_get_bam_seq(fp->fp.cram, &b);
        return ret >= 0
            ? ret
            : (cram_eof(fp->fp.cram) ? -1 : -2);
    }
    default:
        // TODO Need headers available to implement this for SAM files
        hts_log_error("Not implemented for SAM files");
        abort();
    }
}

hts_idx_t *sam_index_load2(htsFile *fp, const char *fn, const char *fnidx)
{
    switch (fp->format.format) {
    case bam:
        return fnidx? hts_idx_load2(fn, fnidx) : hts_idx_load(fn, HTS_FMT_BAI);

    case cram: {
        if (cram_index_load(fp->fp.cram, fn, fnidx) < 0) return NULL;
        // Cons up a fake "index" just pointing at the associated cram_fd:
        hts_cram_idx_t *idx = malloc(sizeof (hts_cram_idx_t));
        if (idx == NULL) return NULL;
        idx->fmt = HTS_FMT_CRAI;
        idx->cram = fp->fp.cram;
        return (hts_idx_t *) idx;
        }

    default:
        return NULL; // TODO Would use tbx_index_load if it returned hts_idx_t
    }
}

hts_idx_t *sam_index_load(htsFile *fp, const char *fn)
{
    return sam_index_load2(fp, fn, NULL);
}

static hts_itr_t *cram_itr_query(const hts_idx_t *idx, int tid, int beg, int end, hts_readrec_func *readrec)
{
    const hts_cram_idx_t *cidx = (const hts_cram_idx_t *) idx;
    hts_itr_t *iter = (hts_itr_t *) calloc(1, sizeof(hts_itr_t));
    if (iter == NULL) return NULL;

    // Cons up a dummy iterator for which hts_itr_next() will simply invoke
    // the readrec function:
    iter->is_cram = 1;
    iter->read_rest = 1;
    iter->off = NULL;
    iter->bins.a = NULL;
    iter->readrec = readrec;

    if (tid >= 0 || tid == HTS_IDX_NOCOOR) {
        cram_range r = { tid == HTS_IDX_NOCOOR ? -1 : tid, beg+1, end };
        int ret = cram_set_option(cidx->cram, CRAM_OPT_RANGE, &r);

        iter->curr_off = 0;
        // The following fields are not required by hts_itr_next(), but are
        // filled in in case user code wants to look at them.
        iter->tid = tid;
        iter->beg = beg;
        iter->end = end;

        switch (ret) {
        case 0:
            break;

        case -2:
            // No data vs this ref, so mark iterator as completed.
            // Same as HTS_IDX_NONE.
            iter->finished = 1;
            break;

        default:
            free(iter);
            return NULL;
        }
    }
    else switch (tid) {
    case HTS_IDX_REST:
        iter->curr_off = 0;
        break;
    case HTS_IDX_NONE:
        iter->curr_off = 0;
        iter->finished = 1;
        break;
    default:
        hts_log_error("Query with tid=%d not implemented for CRAM files", tid);
        abort();
        break;
    }

    return iter;
}

hts_itr_t *sam_itr_queryi(const hts_idx_t *idx, int tid, int beg, int end)
{
    const hts_cram_idx_t *cidx = (const hts_cram_idx_t *) idx;
    if (idx == NULL)
        return hts_itr_query(NULL, tid, beg, end, sam_bam_cram_readrec);
    else if (cidx->fmt == HTS_FMT_CRAI)
        return cram_itr_query(idx, tid, beg, end, cram_readrec);
    else
        return hts_itr_query(idx, tid, beg, end, bam_readrec);
}

static int cram_name2id(void *fdv, const char *ref)
{
    cram_fd *fd = (cram_fd *) fdv;
    return sam_hdr_name2ref(fd->header, ref);
}

hts_itr_t *sam_itr_querys(const hts_idx_t *idx, bam_hdr_t *hdr, const char *region)
{
    const hts_cram_idx_t *cidx = (const hts_cram_idx_t *) idx;
    if (cidx->fmt == HTS_FMT_CRAI)
        return hts_itr_querys(idx, region, cram_name2id, cidx->cram, cram_itr_query, cram_readrec);
    else
        return hts_itr_querys(idx, region, (hts_name2id_f)(bam_name2id), hdr, hts_itr_query, bam_readrec);
}

/**********************
 *** SAM header I/O ***
 **********************/

#include "htslib/kseq.h"
#include "htslib/kstring.h"

bam_hdr_t *sam_hdr_parse(int l_text, const char *text)
{
    const char *q, *r, *p;
    khash_t(s2i) *d;
    d = kh_init(s2i);
    for (p = text; *p; ++p) {
        if (strncmp(p, "@SQ\t", 4) == 0) {
            char *sn = 0;
            int ln = -1;
            for (q = p + 4;; ++q) {
                if (strncmp(q, "SN:", 3) == 0) {
                    q += 3;
                    for (r = q; *r != '\t' && *r != '\n' && *r != '\0'; ++r);
                    sn = (char*)calloc(r - q + 1, 1);
                    strncpy(sn, q, r - q);
                    q = r;
                } else if (strncmp(q, "LN:", 3) == 0)
                    ln = strtol(q + 3, (char**)&q, 10);
                while (*q != '\t' && *q != '\n' && *q != '\0') ++q;
                if (*q == '\0' || *q == '\n') break;
            }
            p = q;
            if (sn && ln >= 0) {
                khint_t k;
                int absent;
                k = kh_put(s2i, d, sn, &absent);
                if (!absent) {
                    hts_log_warning("Duplicated sequence '%s'", sn);
                    free(sn);
                } else kh_val(d, k) = (int64_t)(kh_size(d) - 1)<<32 | ln;
            }
        }
        while (*p != '\0' && *p != '\n') ++p;
    }
    return hdr_from_dict(d);
}

// Minimal sanitisation of a header to ensure.
// - null terminated string.
// - all lines start with @ (also implies no blank lines).
//
// Much more could be done, but currently is not, including:
// - checking header types are known (HD, SQ, etc).
// - syntax (eg checking tab separated fields).
// - validating n_targets matches @SQ records.
// - validating target lengths against @SQ records.
static bam_hdr_t *sam_hdr_sanitise(bam_hdr_t *h) {
    if (!h)
        return NULL;

    // Special case for empty headers.
    if (h->l_text == 0)
        return h;

    uint32_t i, lnum = 0;
    char *cp = h->text, last = '\n';
    for (i = 0; i < h->l_text; i++) {
        // NB: l_text excludes terminating nul.  This finds early ones.
        if (cp[i] == 0)
            break;

        // Error on \n[^@], including duplicate newlines
        if (last == '\n') {
            lnum++;
            if (cp[i] != '@') {
                hts_log_error("Malformed SAM header at line %u", lnum);
                bam_hdr_destroy(h);
                return NULL;
            }
        }

        last = cp[i];
    }

    if (i < h->l_text) { // Early nul found.  Complain if not just padding.
        uint32_t j = i;
        while (j < h->l_text && cp[j] == '\0') j++;
        if (j < h->l_text)
            hts_log_warning("Unexpected NUL character in header. Possibly truncated");
    }

    // Add trailing newline and/or trailing nul if required.
    if (last != '\n') {
        hts_log_warning("Missing trailing newline on SAM header. Possibly truncated");

        if (h->l_text == UINT32_MAX) {
            hts_log_error("No room for extra newline");
            bam_hdr_destroy(h);
            return NULL;
        }

        if (i >= h->l_text - 1) {
            cp = realloc(h->text, (size_t) h->l_text+2);
            if (!cp) {
                bam_hdr_destroy(h);
                return NULL;
            }
            h->text = cp;
        }
        cp[i++] = '\n';

        // l_text may be larger already due to multiple nul padding
        if (h->l_text < i)
            h->l_text = i;
        cp[h->l_text] = '\0';
    }

    return h;
}

bam_hdr_t *sam_hdr_read(htsFile *fp)
{
    switch (fp->format.format) {
    case bam:
        return sam_hdr_sanitise(bam_hdr_read(fp->fp.bgzf));

    case cram:
        return sam_hdr_sanitise(cram_header_to_bam(fp->fp.cram->header));

    case sam: {
        kstring_t str = { 0, 0, NULL };
        bam_hdr_t *h = NULL;
        int ret, has_SQ = 0;
        while ((ret = hts_getline(fp, KS_SEP_LINE, &fp->line)) >= 0) {
            if (fp->line.s[0] != '@') break;
            if (fp->line.l > 3 && strncmp(fp->line.s,"@SQ",3) == 0) has_SQ = 1;
            kputsn(fp->line.s, fp->line.l, &str);
            kputc('\n', &str);
        }
        if (ret < -1) goto error;
        if (! has_SQ && fp->fn_aux) {
            kstring_t line = { 0, 0, NULL };
            hFILE *f = hopen(fp->fn_aux, "r");
            if (f == NULL) goto error;
            while (line.l = 0, kgetline(&line, (kgets_func *) hgets, f) >= 0) {
                char *tab = strchr(line.s, '\t');
                if (tab == NULL) continue;
                kputs("@SQ\tSN:", &str);
                kputsn(line.s, tab - line.s, &str);
                kputs("\tLN:", &str);
                kputl(atol(tab), &str);
                kputc('\n', &str);
            }
            free(line.s);
            if (hclose(f) != 0) {
                hts_log_warning("Failed to close %s", fp->fn_aux);
            }
        }
        if (str.l == 0) kputsn("", 0, &str);
        h = sam_hdr_parse(str.l, str.s);
        h->l_text = str.l; h->text = str.s;
        return sam_hdr_sanitise(h);

     error:
        bam_hdr_destroy(h);
        free(str.s);
        return NULL;
        }

    default:
        abort();
    }
}

int sam_hdr_write(htsFile *fp, const bam_hdr_t *h)
{
    if (!h) {
        errno = EINVAL;
        return -1;
    }

    switch (fp->format.format) {
    case binary_format:
        fp->format.category = sequence_data;
        fp->format.format = bam;
        /* fall-through */
    case bam:
        if (bam_hdr_write(fp->fp.bgzf, h) < 0) return -1;
        break;

    case cram: {
        cram_fd *fd = fp->fp.cram;
        SAM_hdr *hdr = bam_header_to_cram((bam_hdr_t *)h);
        if (! hdr) return -1;
        if (cram_set_header(fd, hdr) < 0) return -1;
        if (fp->fn_aux)
            cram_load_reference(fd, fp->fn_aux);
        if (cram_write_SAM_hdr(fd, fd->header) < 0) return -1;
        }
        break;

    case text_format:
        fp->format.category = sequence_data;
        fp->format.format = sam;
        /* fall-through */
    case sam: {
        char *p;
        hputs(h->text, fp->fp.hfile);
        p = strstr(h->text, "@SQ\t"); // FIXME: we need a loop to make sure "@SQ\t" does not match something unwanted!!!
        if (p == 0) {
            int i;
            for (i = 0; i < h->n_targets; ++i) {
                fp->line.l = 0;
                kputsn("@SQ\tSN:", 7, &fp->line); kputs(h->target_name[i], &fp->line);
                kputsn("\tLN:", 4, &fp->line); kputw(h->target_len[i], &fp->line); kputc('\n', &fp->line);
                if ( hwrite(fp->fp.hfile, fp->line.s, fp->line.l) != fp->line.l ) return -1;
            }
        }
        if ( hflush(fp->fp.hfile) != 0 ) return -1;
        }
        break;

    default:
        abort();
    }
    return 0;
}

/**********************
 *** SAM record I/O ***
 **********************/

int sam_parse1(kstring_t *s, bam_hdr_t *h, bam1_t *b)
{
#define _read_token(_p) (_p); for (; *(_p) && *(_p) != '\t'; ++(_p)); if (*(_p) != '\t') goto err_ret; *(_p)++ = 0
#define _read_token_aux(_p) (_p); for (; *(_p) && *(_p) != '\t'; ++(_p)); *(_p)++ = 0 // this is different in that it does not test *(_p)=='\t'
#define _get_mem(type_t, _x, _s, _l) ks_resize((_s), (_s)->l + (_l)); *(_x) = (type_t*)((_s)->s + (_s)->l); (_s)->l += (_l)
#define _parse_err(cond, msg) do { if (cond) { hts_log_error(msg); goto err_ret; } } while (0)
#define _parse_err_param(cond, msg, param) do { if (cond) { hts_log_error(msg, param); goto err_ret; } } while (0)
#define _parse_warn(cond, msg) do { if (cond) { hts_log_warning(msg); } } while (0)

    uint8_t *t;
    char *p = s->s, *q;
    int i;
    kstring_t str;
    bam1_core_t *c = &b->core;

    str.l = b->l_data = 0;
    str.s = (char*)b->data; str.m = b->m_data;
    memset(c, 0, 32);
    if (h->cigar_tab == 0) {
        h->cigar_tab = (int8_t*) malloc(128);
        for (i = 0; i < 128; ++i)
            h->cigar_tab[i] = -1;
        for (i = 0; BAM_CIGAR_STR[i]; ++i)
            h->cigar_tab[(int)BAM_CIGAR_STR[i]] = i;
    }
    // qname
    q = _read_token(p);
    _parse_warn(p - q <= 1, "empty query name");
    _parse_err(p - q > 252, "query name too long");
    kputsn_(q, p - q, &str);
    for (c->l_extranul = 0; str.l % 4 != 0; c->l_extranul++)
        kputc_('\0', &str);
    c->l_qname = p - q + c->l_extranul;
    // flag
    c->flag = strtol(p, &p, 0);
    if (*p++ != '\t') goto err_ret; // malformated flag
    // chr
    q = _read_token(p);
    if (strcmp(q, "*")) {
        _parse_err(h->n_targets == 0, "missing SAM header");
        c->tid = bam_name2id(h, q);
        _parse_warn(c->tid < 0, "urecognized reference name; treated as unmapped");
    } else c->tid = -1;
    // pos
    c->pos = strtol(p, &p, 10) - 1;
    if (*p++ != '\t') goto err_ret;
    if (c->pos < 0 && c->tid >= 0) {
        _parse_warn(1, "mapped query cannot have zero coordinate; treated as unmapped");
        c->tid = -1;
    }
    if (c->tid < 0) c->flag |= BAM_FUNMAP;
    // mapq
    c->qual = strtol(p, &p, 10);
    if (*p++ != '\t') goto err_ret;
    // cigar
    if (*p != '*') {
        uint32_t *cigar;
        size_t n_cigar = 0;
        for (q = p; *p && *p != '\t'; ++p)
            if (!isdigit_c(*p)) ++n_cigar;
        if (*p++ != '\t') goto err_ret;
        _parse_err(n_cigar == 0, "no CIGAR operations");
        _parse_err(n_cigar >= 2147483647, "too many CIGAR operations");
        c->n_cigar = n_cigar;
        _get_mem(uint32_t, &cigar, &str, c->n_cigar * sizeof(uint32_t));
        for (i = 0; i < c->n_cigar; ++i, ++q) {
            int op;
            cigar[i] = strtol(q, &q, 10)<<BAM_CIGAR_SHIFT;
            op = (uint8_t)*q >= 128? -1 : h->cigar_tab[(int)*q];
            _parse_err(op < 0, "unrecognized CIGAR operator");
            cigar[i] |= op;
        }
        // can't use bam_endpos() directly as some fields not yet set up
        i = (!(c->flag&BAM_FUNMAP))? bam_cigar2rlen(c->n_cigar, cigar) : 1;
    } else {
        _parse_warn(!(c->flag&BAM_FUNMAP), "mapped query must have a CIGAR; treated as unmapped");
        c->flag |= BAM_FUNMAP;
        q = _read_token(p);
        i = 1;
    }
    c->bin = hts_reg2bin(c->pos, c->pos + i, 14, 5);
    // mate chr
    q = _read_token(p);
    if (strcmp(q, "=") == 0) {
        c->mtid = c->tid;
    } else if (strcmp(q, "*") == 0) {
        c->mtid = -1;
    } else {
        c->mtid = bam_name2id(h, q);
        _parse_warn(c->mtid < 0, "urecognized mate reference name; treated as unmapped");
    }
    // mpos
    c->mpos = strtol(p, &p, 10) - 1;
    if (*p++ != '\t') goto err_ret;
    if (c->mpos < 0 && c->mtid >= 0) {
        _parse_warn(1, "mapped mate cannot have zero coordinate; treated as unmapped");
        c->mtid = -1;
    }
    // tlen
    c->isize = strtol(p, &p, 10);
    if (*p++ != '\t') goto err_ret;
    // seq
    q = _read_token(p);
    if (strcmp(q, "*")) {
        c->l_qseq = p - q - 1;
        i = bam_cigar2qlen(c->n_cigar, (uint32_t*)(str.s + c->l_qname));
        _parse_err(c->n_cigar && i != c->l_qseq, "CIGAR and query sequence are of different length");
        i = (c->l_qseq + 1) >> 1;
        _get_mem(uint8_t, &t, &str, i);
        memset(t, 0, i);
        for (i = 0; i < c->l_qseq; ++i)
            t[i>>1] |= seq_nt16_table[(int)q[i]] << ((~i&1)<<2);
    } else c->l_qseq = 0;
    // qual
    q = _read_token_aux(p);
    _get_mem(uint8_t, &t, &str, c->l_qseq);
    if (strcmp(q, "*")) {
        _parse_err(p - q - 1 != c->l_qseq, "SEQ and QUAL are of different length");
        for (i = 0; i < c->l_qseq; ++i) t[i] = q[i] - 33;
    } else memset(t, 0xff, c->l_qseq);
    // aux
    while (p < s->s + s->l) {
        uint8_t type;
        q = _read_token_aux(p); // FIXME: can be accelerated for long 'B' arrays
        _parse_err(p - q - 1 < 5, "incomplete aux field");
        kputsn_(q, 2, &str);
        q += 3; type = *q++; ++q; // q points to value
        if (type != 'Z' && type != 'H') // the only zero length acceptable fields
            _parse_err(p - q - 1 < 1, "incomplete aux field");

        // Ensure str has enough space for a double + type allocated.
        // This is so we can stuff bigger integers and floats directly into
        // the kstring.  Sorry.
        _parse_err(ks_resize(&str, str.l + 16), "out of memory");

        if (type == 'A' || type == 'a' || type == 'c' || type == 'C') {
            kputc_('A', &str);
            kputc_(*q, &str);
        } else if (type == 'i' || type == 'I') {
            if (*q == '-') {
                long x = strtol(q, &q, 10);
                if (x >= INT8_MIN) {
                    kputc_('c', &str); kputc_(x, &str);
                } else if (x >= INT16_MIN) {
                    str.s[str.l++] = 's';
                    i16_to_le(x, (uint8_t *) str.s + str.l);
                    str.l += 2;
                } else {
                    str.s[str.l++] = 'i';
                    i32_to_le(x, (uint8_t *) str.s + str.l);
                    str.l += 4;
                }
            } else {
                unsigned long x = strtoul(q, &q, 10);
                if (x <= UINT8_MAX) {
                    kputc_('C', &str); kputc_(x, &str);
                } else if (x <= UINT16_MAX) {
                    str.s[str.l++] = 'S';
                    u16_to_le(x, (uint8_t *) str.s + str.l);
                    str.l += 2;
                } else {
                    str.s[str.l++] = 'I';
                    u32_to_le(x, (uint8_t *) str.s + str.l);
                    str.l += 4;
                }
            }
        } else if (type == 'f') {
            str.s[str.l++] = 'f';
            float_to_le(strtod(q, &q), (uint8_t *) str.s + str.l);
            str.l += sizeof(float);
        } else if (type == 'd') {
            str.s[str.l++] = 'd';
            double_to_le(strtod(q, &q), (uint8_t *) str.s + str.l);
            str.l += sizeof(double);
        } else if (type == 'Z' || type == 'H') {
            _parse_err(type == 'H' && !((p-q)&1),
                       "hex field does not have an even number of digits");
            kputc_(type, &str);kputsn_(q, p - q, &str); // note that this include the trailing NULL
        } else if (type == 'B') {
            int32_t n, size;
            size_t bytes;
            char *r;
            _parse_err(p - q - 1 < 3, "incomplete B-typed aux field");
            type = *q++; // q points to the first ',' following the typing byte

            size = aux_type2size(type);
            _parse_err_param(size <= 0 || size > 4,
                             "unrecognized type B:%c", type);
            _parse_err(*q && *q != ',', "B aux field type not followed by ','");

            for (r = q, n = 0; *r; ++r)
                if (*r == ',') ++n;

            // Ensure space for type + values
            bytes = (size_t) n * (size_t) size;
            _parse_err(bytes / size != n
                       || ks_resize(&str, str.l + bytes + 2 + sizeof(uint32_t)),
                       "out of memory");
            str.s[str.l++] = 'B';
            str.s[str.l++] = type;
            i32_to_le(n, (uint8_t *) str.s + str.l);
            str.l += sizeof(uint32_t);

            // This ensures that q always ends up at the next comma after
            // reading a number even if it's followed by junk.  It
            // prevents the possibility of trying to read more than n items.
#define _skip_to_comma(q, p) do { while ((q) < (p) && *(q) != ',') (q)++; } while (0)

            if (type == 'c')      while (q + 1 < p) { int8_t   x = strtol(q + 1, &q, 0); kputc_(x, &str); }
            else if (type == 'C') while (q + 1 < p) { uint8_t  x = strtoul(q + 1, &q, 0); kputc_(x, &str); }
            else if (type == 's') while (q + 1 < p) { i16_to_le(strtol(q + 1, &q, 0), (uint8_t *) str.s + str.l); str.l += 2; _skip_to_comma(q, p); }
            else if (type == 'S') while (q + 1 < p) { u16_to_le(strtoul(q + 1, &q, 0), (uint8_t *) str.s + str.l); str.l += 2; _skip_to_comma(q, p); }
            else if (type == 'i') while (q + 1 < p) { i32_to_le(strtol(q + 1, &q, 0), (uint8_t *) str.s + str.l); str.l += 4; _skip_to_comma(q, p); }
            else if (type == 'I') while (q + 1 < p) { u32_to_le(strtoul(q + 1, &q, 0), (uint8_t *) str.s + str.l); str.l += 4; _skip_to_comma(q, p); }
            else if (type == 'f') while (q + 1 < p) { float_to_le(strtod(q + 1, &q), (uint8_t *) str.s + str.l); str.l += 4; _skip_to_comma(q, p); }
            else _parse_err_param(1, "unrecognized type B:%c", type);

#undef _skip_to_comma

        } else _parse_err_param(1, "unrecognized type %c", type);
    }
    b->data = (uint8_t*)str.s; b->l_data = str.l; b->m_data = str.m;
    return 0;

#undef _parse_warn
#undef _parse_err
#undef _parse_err_param
#undef _get_mem
#undef _read_token_aux
#undef _read_token
err_ret:
    b->data = (uint8_t*)str.s; b->l_data = str.l; b->m_data = str.m;
    return -2;
}

int sam_read1(htsFile *fp, bam_hdr_t *h, bam1_t *b)
{
    switch (fp->format.format) {
    case bam: {
        int r = bam_read1(fp->fp.bgzf, b);
        if (r >= 0) {
            if (b->core.tid  >= h->n_targets || b->core.tid  < -1 ||
                b->core.mtid >= h->n_targets || b->core.mtid < -1)
                return -3;
        }
        return r;
        }

    case cram: {
        int ret = cram_get_bam_seq(fp->fp.cram, &b);
        return ret >= 0
            ? ret
            : (cram_eof(fp->fp.cram) ? -1 : -2);
    }

    case sam: {
        int ret;
err_recover:
        if (fp->line.l == 0) {
            ret = hts_getline(fp, KS_SEP_LINE, &fp->line);
            if (ret < 0) return ret;
        }
        ret = sam_parse1(&fp->line, h, b);
        fp->line.l = 0;
        if (ret < 0) {
            hts_log_warning("Parse error at line %lld", (long long)fp->lineno);
            if (h->ignore_sam_err) goto err_recover;
        }
        return ret;
    }

    default:
        abort();
    }
}

int sam_format1(const bam_hdr_t *h, const bam1_t *b, kstring_t *str)
{
    int i;
    uint8_t *s, *end;
    const bam1_core_t *c = &b->core;

    str->l = 0;
    kputsn(bam_get_qname(b), c->l_qname-1-c->l_extranul, str); kputc('\t', str); // query name
    kputw(c->flag, str); kputc('\t', str); // flag
    if (c->tid >= 0) { // chr
        kputs(h->target_name[c->tid] , str);
        kputc('\t', str);
    } else kputsn("*\t", 2, str);
    kputw(c->pos + 1, str); kputc('\t', str); // pos
    kputw(c->qual, str); kputc('\t', str); // qual
    if (c->n_cigar) { // cigar
        uint32_t *cigar = bam_get_cigar(b);
        for (i = 0; i < c->n_cigar; ++i) {
            kputw(bam_cigar_oplen(cigar[i]), str);
            kputc(bam_cigar_opchr(cigar[i]), str);
        }
    } else kputc('*', str);
    kputc('\t', str);
    if (c->mtid < 0) kputsn("*\t", 2, str); // mate chr
    else if (c->mtid == c->tid) kputsn("=\t", 2, str);
    else {
        kputs(h->target_name[c->mtid], str);
        kputc('\t', str);
    }
    kputw(c->mpos + 1, str); kputc('\t', str); // mate pos
    kputw(c->isize, str); kputc('\t', str); // template len
    if (c->l_qseq) { // seq and qual
        uint8_t *s = bam_get_seq(b);
        for (i = 0; i < c->l_qseq; ++i) kputc("=ACMGRSVTWYHKDBN"[bam_seqi(s, i)], str);
        kputc('\t', str);
        s = bam_get_qual(b);
        if (s[0] == 0xff) kputc('*', str);
        else for (i = 0; i < c->l_qseq; ++i) kputc(s[i] + 33, str);
    } else kputsn("*\t*", 3, str);

    s = bam_get_aux(b); // aux
    end = b->data + b->l_data;
    while (end - s >= 4) {
        uint8_t type, key[2];
        key[0] = s[0]; key[1] = s[1];
        s += 2; type = *s++;
        kputc('\t', str); kputsn((char*)key, 2, str); kputc(':', str);
        if (type == 'A') {
            kputsn("A:", 2, str);
            kputc(*s, str);
            ++s;
        } else if (type == 'C') {
            kputsn("i:", 2, str);
            kputw(*s, str);
            ++s;
        } else if (type == 'c') {
            kputsn("i:", 2, str);
            kputw(*(int8_t*)s, str);
            ++s;
        } else if (type == 'S') {
            if (end - s >= 2) {
                kputsn("i:", 2, str);
                kputuw(le_to_u16(s), str);
                s += 2;
            } else goto bad_aux;
        } else if (type == 's') {
            if (end - s >= 2) {
                kputsn("i:", 2, str);
                kputw(le_to_i16(s), str);
                s += 2;
            } else goto bad_aux;
        } else if (type == 'I') {
            if (end - s >= 4) {
                kputsn("i:", 2, str);
                kputuw(le_to_u32(s), str);
                s += 4;
            } else goto bad_aux;
        } else if (type == 'i') {
            if (end - s >= 4) {
                kputsn("i:", 2, str);
                kputw(le_to_i32(s), str);
                s += 4;
            } else goto bad_aux;
        } else if (type == 'f') {
            if (end - s >= 4) {
                ksprintf(str, "f:%g", le_to_float(s));
                s += 4;
            } else goto bad_aux;

        } else if (type == 'd') {
            if (end - s >= 8) {
                ksprintf(str, "d:%g", le_to_double(s));
                s += 8;
            } else goto bad_aux;
        } else if (type == 'Z' || type == 'H') {
            kputc(type, str); kputc(':', str);
            while (s < end && *s) kputc(*s++, str);
            if (s >= end)
                goto bad_aux;
            ++s;
        } else if (type == 'B') {
            uint8_t sub_type = *(s++);
            int sub_type_size = aux_type2size(sub_type);
            uint32_t n;
            if (sub_type_size == 0 || end - s < 4)
                goto bad_aux;
            n = le_to_u32(s);
            s += 4; // now points to the start of the array
            if ((end - s) / sub_type_size < n)
                goto bad_aux;
            kputsn("B:", 2, str); kputc(sub_type, str); // write the typing
            for (i = 0; i < n; ++i) { // FIXME: for better performance, put the loop after "if"
                kputc(',', str);
                if ('c' == sub_type)      { kputw(*(int8_t*)s, str); ++s; }
                else if ('C' == sub_type) { kputw(*(uint8_t*)s, str); ++s; }
                else if ('s' == sub_type) { kputw(le_to_i16(s), str); s += 2; }
                else if ('S' == sub_type) { kputw(le_to_u16(s), str); s += 2; }
                else if ('i' == sub_type) { kputw(le_to_i32(s), str); s += 4; }
                else if ('I' == sub_type) { kputuw(le_to_u32(s), str); s += 4; }
                else if ('f' == sub_type) { kputd(le_to_float(s), str); s += 4; }
                else goto bad_aux;  // Unknown sub-type
            }
        } else { // Unknown type
            goto bad_aux;
        }
    }
    return str->l;

 bad_aux:
    hts_log_error("Corrupted aux data for read %.*s",
                  b->core.l_qname, bam_get_qname(b));
    errno = EINVAL;
    return -1;
}

int sam_write1(htsFile *fp, const bam_hdr_t *h, const bam1_t *b)
{
    switch (fp->format.format) {
    case binary_format:
        fp->format.category = sequence_data;
        fp->format.format = bam;
        /* fall-through */
    case bam:
        return bam_write1(fp->fp.bgzf, b);

    case cram:
        return cram_put_bam_seq(fp->fp.cram, (bam1_t *)b);

    case text_format:
        fp->format.category = sequence_data;
        fp->format.format = sam;
        /* fall-through */
    case sam:
        if (sam_format1(h, b, &fp->line) < 0) return -1;
        kputc('\n', &fp->line);
        if ( hwrite(fp->fp.hfile, fp->line.s, fp->line.l) != fp->line.l ) return -1;
        return fp->line.l;

    default:
        abort();
    }
}

/************************
 *** Auxiliary fields ***
 ************************/
#ifndef HTS_LITTLE_ENDIAN
static int aux_to_le(char type, uint8_t *out, const uint8_t *in, size_t len) {
    int tsz = aux_type2size(type);

    if (tsz >= 2 && tsz <= 8 && (len & (tsz - 1)) != 0) return -1;

    switch (tsz) {
        case 'H': case 'Z': case 1:  // Trivial
            memcpy(out, in, len);
            break;

#define aux_val_to_le(type_t, store_le) do {                            \
        type_t v;                                                       \
        size_t i;                                                       \
        for (i = 0; i < len; i += sizeof(type_t), out += sizeof(type_t)) { \
            memcpy(&v, in + i, sizeof(type_t));                         \
            store_le(v, out);                                           \
        }                                                               \
    } while (0)

        case 2: aux_val_to_le(uint16_t, u16_to_le); break;
        case 4: aux_val_to_le(uint32_t, u32_to_le); break;
        case 8: aux_val_to_le(uint64_t, u64_to_le); break;

#undef aux_val_to_le

        case 'B': { // Recurse!
            uint32_t n;
            if (len < 5) return -1;
            memcpy(&n, in + 1, 4);
            out[0] = in[0];
            u32_to_le(n, out + 1);
            return aux_to_le(in[0], out + 5, in + 5, len - 5);
        }

        default: // Unknown type code
            return -1;
    }



    return 0;
}
#endif

int bam_aux_append(bam1_t *b, const char tag[2], char type, int len, const uint8_t *data)
{
    uint32_t new_len;

    assert(b->l_data >= 0);
    new_len = b->l_data + 3 + len;
    if (new_len > INT32_MAX || new_len < b->l_data) goto nomem;

    if (b->m_data < new_len) {
        uint32_t new_size = new_len;
        uint8_t *new_data;
        kroundup32(new_size);
        new_data = realloc(b->data, new_size);
        if (new_data == NULL) goto nomem;
        b->m_data = new_size;
        b->data = new_data;
    }

    b->data[b->l_data] = tag[0];
    b->data[b->l_data + 1] = tag[1];
    b->data[b->l_data + 2] = type;

#ifdef HTS_LITTLE_ENDIAN
    memcpy(b->data + b->l_data + 3, data, len);
#else
    if (aux_to_le(type, b->data + b->l_data + 3, data, len) != 0) {
        errno = EINVAL;
        return -1;
    }
#endif

    b->l_data = new_len;

    return 0;

 nomem:
    errno = ENOMEM;
    return -1;
}

static inline uint8_t *skip_aux(uint8_t *s, uint8_t *end)
{
    int size;
    uint32_t n;
    if (s >= end) return end;
    size = aux_type2size(*s); ++s; // skip type
    switch (size) {
    case 'Z':
    case 'H':
        while (*s && s < end) ++s;
        return s < end ? s + 1 : end;
    case 'B':
        if (end - s < 5) return NULL;
        size = aux_type2size(*s); ++s;
        n = le_to_u32(s);
        s += 4;
        if (size == 0 || end - s < size * n) return NULL;
        return s + size * n;
    case 0:
        return NULL;
    default:
        if (end - s < size) return NULL;
        return s + size;
    }
}

uint8_t *bam_aux_get(const bam1_t *b, const char tag[2])
{
    uint8_t *s, *end, *t = (uint8_t *) tag;
    uint16_t y = (uint16_t) t[0]<<8 | t[1];
    s = bam_get_aux(b);
    end = b->data + b->l_data;
    while (s != NULL && end - s >= 3) {
        uint16_t x = (uint16_t) s[0]<<8 | s[1];
        s += 2;
        if (x == y) {
            // Check the tag value is valid and complete
            uint8_t *e = skip_aux(s, end);
            if ((*s == 'Z' || *s == 'H') && *(e - 1) != '\0') {
                goto bad_aux;  // Unterminated string
            }
            if (e != NULL) {
                return s;
            } else {
                goto bad_aux;
            }
        }
        s = skip_aux(s, end);
    }
    if (s == NULL) goto bad_aux;
    errno = ENOENT;
    return NULL;

 bad_aux:
    hts_log_error("Corrupted aux data for read %s", bam_get_qname(b));
    errno = EINVAL;
    return NULL;
}
// s MUST BE returned by bam_aux_get()
int bam_aux_del(bam1_t *b, uint8_t *s)
{
    uint8_t *p, *aux;
    int l_aux = bam_get_l_aux(b);
    aux = bam_get_aux(b);
    p = s - 2;
    s = skip_aux(s, aux + l_aux);
    if (s == NULL) goto bad_aux;
    memmove(p, s, l_aux - (s - aux));
    b->l_data -= s - p;
    return 0;

 bad_aux:
    hts_log_error("Corrupted aux data for read %s", bam_get_qname(b));
    errno = EINVAL;
    return -1;
}

int bam_aux_update_str(bam1_t *b, const char tag[2], int len, const char *data)
{
    // FIXME: This is not at all efficient!
    uint8_t *s = bam_aux_get(b,tag);
    if (!s) {
        if (errno == ENOENT) {  // Tag doesn't exist - add a new one
            return bam_aux_append(b, tag, 'Z', len, (const uint8_t *) data);
        } else { // Invalid aux data, give up.
            return -1;
        }
    }
    char type = *s;
    if (type != 'Z') {
        hts_log_error("Called bam_aux_update_str for type '%c' instead of 'Z'", type);
        errno = EINVAL;
        return -1;
    }

    bam_aux_del(b,s);
    s -= 2;
    int l_aux = bam_get_l_aux(b);

    b->l_data += 3 + len;
    if (b->m_data < b->l_data) {
        ptrdiff_t s_offset = s - b->data;
        b->m_data = b->l_data;
        kroundup32(b->m_data);
        b->data = (uint8_t*)realloc(b->data, b->m_data);
        s = b->data + s_offset;
    }
    memmove(s+3+len, s, l_aux - (s - bam_get_aux(b)));
    s[0] = tag[0];
    s[1] = tag[1];
    s[2] = type;
    memmove(s+3,data,len);
    return 0;
}

static inline int64_t get_int_aux_val(uint8_t type, const uint8_t *s,
                                      uint32_t idx)
{
    switch (type) {
        case 'c': return le_to_i8(s + idx);
        case 'C': return s[idx];
        case 's': return le_to_i16(s + 2 * idx);
        case 'S': return le_to_u16(s + 2 * idx);
        case 'i': return le_to_i32(s + 4 * idx);
        case 'I': return le_to_u32(s + 4 * idx);
        default:
            errno = EINVAL;
            return 0;
    }
}

int64_t bam_aux2i(const uint8_t *s)
{
    int type;
    type = *s++;
    return get_int_aux_val(type, s, 0);
}

double bam_aux2f(const uint8_t *s)
{
    int type;
    type = *s++;
    if (type == 'd') return le_to_double(s);
    else if (type == 'f') return le_to_float(s);
    else return get_int_aux_val(type, s, 0);
}

char bam_aux2A(const uint8_t *s)
{
    int type;
    type = *s++;
    if (type == 'A') return *(char*)s;
    errno = EINVAL;
    return 0;
}

char *bam_aux2Z(const uint8_t *s)
{
    int type;
    type = *s++;
    if (type == 'Z' || type == 'H') return (char*)s;
    errno = EINVAL;
    return 0;
}

uint32_t bam_auxB_len(const uint8_t *s)
{
    if (s[0] != 'B') {
        errno = EINVAL;
        return 0;
    }
    return le_to_u32(s + 2);
}

int64_t bam_auxB2i(const uint8_t *s, uint32_t idx)
{
    uint32_t len = bam_auxB_len(s);
    if (idx >= len) {
        errno = ERANGE;
        return 0;
    }
    return get_int_aux_val(s[1], s + 6, idx);
}

double bam_auxB2f(const uint8_t *s, uint32_t idx)
{
    uint32_t len = bam_auxB_len(s);
    if (idx >= len) {
        errno = ERANGE;
        return 0.0;
    }
    if (s[1] == 'f') return le_to_float(s + 6 + 4 * idx);
    else return get_int_aux_val(s[1], s + 6, idx);
}

int sam_open_mode(char *mode, const char *fn, const char *format)
{
    // TODO Parse "bam5" etc for compression level
    if (format == NULL) {
        // Try to pick a format based on the filename extension
        const char *ext = fn? strrchr(fn, '.') : NULL;
        if (ext == NULL || strchr(ext, '/')) return -1;
        return sam_open_mode(mode, fn, ext+1);
    }
    else if (strcmp(format, "bam") == 0) strcpy(mode, "b");
    else if (strcmp(format, "cram") == 0) strcpy(mode, "c");
    else if (strcmp(format, "sam") == 0) strcpy(mode, "");
    else return -1;

    return 0;
}

// A version of sam_open_mode that can handle ,key=value options.
// The format string is allocated and returned, to be freed by the caller.
// Prefix should be "r" or "w",
char *sam_open_mode_opts(const char *fn,
                         const char *mode,
                         const char *format)
{
    char *mode_opts = malloc((format ? strlen(format) : 1) +
                             (mode   ? strlen(mode)   : 1) + 12);
    char *opts, *cp;
    int format_len;

    if (!mode_opts)
        return NULL;

    strcpy(mode_opts, mode ? mode : "r");
    cp = mode_opts + strlen(mode_opts);

    if (format == NULL) {
        // Try to pick a format based on the filename extension
        const char *ext = fn? strrchr(fn, '.') : NULL;
        if (ext == NULL || strchr(ext, '/')) {
            free(mode_opts);
            return NULL;
        }
        return sam_open_mode(cp, fn, ext+1)
            ? (free(mode_opts), NULL)
            : mode_opts;
    }

    if ((opts = strchr(format, ','))) {
        format_len = opts-format;
    } else {
        opts="";
        format_len = strlen(format);
    }

    if (strncmp(format, "bam", format_len) == 0) {
        *cp++ = 'b';
    } else if (strncmp(format, "cram", format_len) == 0) {
        *cp++ = 'c';
    } else if (strncmp(format, "cram2", format_len) == 0) {
        *cp++ = 'c';
        strcpy(cp, ",VERSION=2.1");
        cp += 12;
    } else if (strncmp(format, "cram3", format_len) == 0) {
        *cp++ = 'c';
        strcpy(cp, ",VERSION=3.0");
        cp += 12;
    } else if (strncmp(format, "sam", format_len) == 0) {
        ; // format mode=""
    } else {
        free(mode_opts);
        return NULL;
    }

    strcpy(cp, opts);

    return mode_opts;
}

#define STRNCMP(a,b,n) (strncasecmp((a),(b),(n)) || strlen(a)!=(n))
int bam_str2flag(const char *str)
{
    char *end, *beg = (char*) str;
    long int flag = strtol(str, &end, 0);
    if ( end!=str ) return flag;    // the conversion was successful
    flag = 0;
    while ( *str )
    {
        end = beg;
        while ( *end && *end!=',' ) end++;
        if ( !STRNCMP("PAIRED",beg,end-beg) ) flag |= BAM_FPAIRED;
        else if ( !STRNCMP("PROPER_PAIR",beg,end-beg) ) flag |= BAM_FPROPER_PAIR;
        else if ( !STRNCMP("UNMAP",beg,end-beg) ) flag |= BAM_FUNMAP;
        else if ( !STRNCMP("MUNMAP",beg,end-beg) ) flag |= BAM_FMUNMAP;
        else if ( !STRNCMP("REVERSE",beg,end-beg) ) flag |= BAM_FREVERSE;
        else if ( !STRNCMP("MREVERSE",beg,end-beg) ) flag |= BAM_FMREVERSE;
        else if ( !STRNCMP("READ1",beg,end-beg) ) flag |= BAM_FREAD1;
        else if ( !STRNCMP("READ2",beg,end-beg) ) flag |= BAM_FREAD2;
        else if ( !STRNCMP("SECONDARY",beg,end-beg) ) flag |= BAM_FSECONDARY;
        else if ( !STRNCMP("QCFAIL",beg,end-beg) ) flag |= BAM_FQCFAIL;
        else if ( !STRNCMP("DUP",beg,end-beg) ) flag |= BAM_FDUP;
        else if ( !STRNCMP("SUPPLEMENTARY",beg,end-beg) ) flag |= BAM_FSUPPLEMENTARY;
        else return -1;
        if ( !*end ) break;
        beg = end + 1;
    }
    return flag;
}

char *bam_flag2str(int flag)
{
    kstring_t str = {0,0,0};
    if ( flag&BAM_FPAIRED ) ksprintf(&str,"%s%s", str.l?",":"","PAIRED");
    if ( flag&BAM_FPROPER_PAIR ) ksprintf(&str,"%s%s", str.l?",":"","PROPER_PAIR");
    if ( flag&BAM_FUNMAP ) ksprintf(&str,"%s%s", str.l?",":"","UNMAP");
    if ( flag&BAM_FMUNMAP ) ksprintf(&str,"%s%s", str.l?",":"","MUNMAP");
    if ( flag&BAM_FREVERSE ) ksprintf(&str,"%s%s", str.l?",":"","REVERSE");
    if ( flag&BAM_FMREVERSE ) ksprintf(&str,"%s%s", str.l?",":"","MREVERSE");
    if ( flag&BAM_FREAD1 ) ksprintf(&str,"%s%s", str.l?",":"","READ1");
    if ( flag&BAM_FREAD2 ) ksprintf(&str,"%s%s", str.l?",":"","READ2");
    if ( flag&BAM_FSECONDARY ) ksprintf(&str,"%s%s", str.l?",":"","SECONDARY");
    if ( flag&BAM_FQCFAIL ) ksprintf(&str,"%s%s", str.l?",":"","QCFAIL");
    if ( flag&BAM_FDUP ) ksprintf(&str,"%s%s", str.l?",":"","DUP");
    if ( flag&BAM_FSUPPLEMENTARY ) ksprintf(&str,"%s%s", str.l?",":"","SUPPLEMENTARY");
    if ( str.l == 0 ) kputsn("", 0, &str);
    return str.s;
}


/**************************
 *** Pileup and Mpileup ***
 **************************/

#if !defined(BAM_NO_PILEUP)

#include <assert.h>

/*******************
 *** Memory pool ***
 *******************/

typedef struct {
    int k, x, y, end;
} cstate_t;

static cstate_t g_cstate_null = { -1, 0, 0, 0 };

typedef struct __linkbuf_t {
    bam1_t b;
    int32_t beg, end;
    cstate_t s;
    struct __linkbuf_t *next;
    bam_pileup_cd cd;
} lbnode_t;

typedef struct {
    int cnt, n, max;
    lbnode_t **buf;
} mempool_t;

static mempool_t *mp_init(void)
{
    mempool_t *mp;
    mp = (mempool_t*)calloc(1, sizeof(mempool_t));
    return mp;
}
static void mp_destroy(mempool_t *mp)
{
    int k;
    for (k = 0; k < mp->n; ++k) {
        free(mp->buf[k]->b.data);
        free(mp->buf[k]);
    }
    free(mp->buf);
    free(mp);
}
static inline lbnode_t *mp_alloc(mempool_t *mp)
{
    ++mp->cnt;
    if (mp->n == 0) return (lbnode_t*)calloc(1, sizeof(lbnode_t));
    else return mp->buf[--mp->n];
}
static inline void mp_free(mempool_t *mp, lbnode_t *p)
{
    --mp->cnt; p->next = 0; // clear lbnode_t::next here
    if (mp->n == mp->max) {
        mp->max = mp->max? mp->max<<1 : 256;
        mp->buf = (lbnode_t**)realloc(mp->buf, sizeof(lbnode_t*) * mp->max);
    }
    mp->buf[mp->n++] = p;
}

/**********************
 *** CIGAR resolver ***
 **********************/

/* s->k: the index of the CIGAR operator that has just been processed.
   s->x: the reference coordinate of the start of s->k
   s->y: the query coordiante of the start of s->k
 */
static inline int resolve_cigar2(bam_pileup1_t *p, int32_t pos, cstate_t *s)
{
#define _cop(c) ((c)&BAM_CIGAR_MASK)
#define _cln(c) ((c)>>BAM_CIGAR_SHIFT)

    bam1_t *b = p->b;
    bam1_core_t *c = &b->core;
    uint32_t *cigar = bam_get_cigar(b);
    int k;
    // determine the current CIGAR operation
    //fprintf(stderr, "%s\tpos=%d\tend=%d\t(%d,%d,%d)\n", bam_get_qname(b), pos, s->end, s->k, s->x, s->y);
    if (s->k == -1) { // never processed
        if (c->n_cigar == 1) { // just one operation, save a loop
          if (_cop(cigar[0]) == BAM_CMATCH || _cop(cigar[0]) == BAM_CEQUAL || _cop(cigar[0]) == BAM_CDIFF) s->k = 0, s->x = c->pos, s->y = 0;
        } else { // find the first match or deletion
            for (k = 0, s->x = c->pos, s->y = 0; k < c->n_cigar; ++k) {
                int op = _cop(cigar[k]);
                int l = _cln(cigar[k]);
                if (op == BAM_CMATCH || op == BAM_CDEL || op == BAM_CEQUAL || op == BAM_CDIFF) break;
                else if (op == BAM_CREF_SKIP) s->x += l;
                else if (op == BAM_CINS || op == BAM_CSOFT_CLIP) s->y += l;
            }
            assert(k < c->n_cigar);
            s->k = k;
        }
    } else { // the read has been processed before
        int op, l = _cln(cigar[s->k]);
        if (pos - s->x >= l) { // jump to the next operation
            assert(s->k < c->n_cigar); // otherwise a bug: this function should not be called in this case
            op = _cop(cigar[s->k+1]);
            if (op == BAM_CMATCH || op == BAM_CDEL || op == BAM_CREF_SKIP || op == BAM_CEQUAL || op == BAM_CDIFF) { // jump to the next without a loop
              if (_cop(cigar[s->k]) == BAM_CMATCH|| _cop(cigar[s->k]) == BAM_CEQUAL || _cop(cigar[s->k]) == BAM_CDIFF) s->y += l;
                s->x += l;
                ++s->k;
            } else { // find the next M/D/N/=/X
              if (_cop(cigar[s->k]) == BAM_CMATCH|| _cop(cigar[s->k]) == BAM_CEQUAL || _cop(cigar[s->k]) == BAM_CDIFF) s->y += l;
                s->x += l;
                for (k = s->k + 1; k < c->n_cigar; ++k) {
                    op = _cop(cigar[k]), l = _cln(cigar[k]);
                    if (op == BAM_CMATCH || op == BAM_CDEL || op == BAM_CREF_SKIP || op == BAM_CEQUAL || op == BAM_CDIFF) break;
                    else if (op == BAM_CINS || op == BAM_CSOFT_CLIP) s->y += l;
                }
                s->k = k;
            }
            assert(s->k < c->n_cigar); // otherwise a bug
        } // else, do nothing
    }
    { // collect pileup information
        int op, l;
        op = _cop(cigar[s->k]); l = _cln(cigar[s->k]);
        p->is_del = p->indel = p->is_refskip = 0;
        if (s->x + l - 1 == pos && s->k + 1 < c->n_cigar) { // peek the next operation
            int op2 = _cop(cigar[s->k+1]);
            int l2 = _cln(cigar[s->k+1]);
            if (op2 == BAM_CDEL) p->indel = -(int)l2;
            else if (op2 == BAM_CINS) p->indel = l2;
            else if (op2 == BAM_CPAD && s->k + 2 < c->n_cigar) { // no working for adjacent padding
                int l3 = 0;
                for (k = s->k + 2; k < c->n_cigar; ++k) {
                    op2 = _cop(cigar[k]); l2 = _cln(cigar[k]);
                    if (op2 == BAM_CINS) l3 += l2;
                    else if (op2 == BAM_CDEL || op2 == BAM_CMATCH || op2 == BAM_CREF_SKIP || op2 == BAM_CEQUAL || op2 == BAM_CDIFF) break;
                }
                if (l3 > 0) p->indel = l3;
            }
        }
        if (op == BAM_CMATCH || op == BAM_CEQUAL || op == BAM_CDIFF) {
            p->qpos = s->y + (pos - s->x);
        } else if (op == BAM_CDEL || op == BAM_CREF_SKIP) {
            p->is_del = 1; p->qpos = s->y; // FIXME: distinguish D and N!!!!!
            p->is_refskip = (op == BAM_CREF_SKIP);
        } // cannot be other operations; otherwise a bug
        p->is_head = (pos == c->pos); p->is_tail = (pos == s->end);
    }
    return 1;
}

/***********************
 *** Pileup iterator ***
 ***********************/

// Dictionary of overlapping reads
KHASH_MAP_INIT_STR(olap_hash, lbnode_t *)
typedef khash_t(olap_hash) olap_hash_t;

struct __bam_plp_t {
    mempool_t *mp;
    lbnode_t *head, *tail;
    int32_t tid, pos, max_tid, max_pos;
    int is_eof, max_plp, error, maxcnt;
    uint64_t id;
    bam_pileup1_t *plp;
    // for the "auto" interface only
    bam1_t *b;
    bam_plp_auto_f func;
    void *data;
    olap_hash_t *overlaps;

    // For notification of creation and destruction events
    // and associated client-owned pointer.
    int (*plp_construct)(void *data, const bam1_t *b, bam_pileup_cd *cd);
    int (*plp_destruct )(void *data, const bam1_t *b, bam_pileup_cd *cd);
};

bam_plp_t bam_plp_init(bam_plp_auto_f func, void *data)
{
    bam_plp_t iter;
    iter = (bam_plp_t)calloc(1, sizeof(struct __bam_plp_t));
    iter->mp = mp_init();
    iter->head = iter->tail = mp_alloc(iter->mp);
    iter->max_tid = iter->max_pos = -1;
    iter->maxcnt = 8000;
    if (func) {
        iter->func = func;
        iter->data = data;
        iter->b = bam_init1();
    }
    return iter;
}

void bam_plp_init_overlaps(bam_plp_t iter)
{
    iter->overlaps = kh_init(olap_hash);  // hash for tweaking quality of bases in overlapping reads
}

void bam_plp_destroy(bam_plp_t iter)
{
    lbnode_t *p, *pnext;
    if ( iter->overlaps ) kh_destroy(olap_hash, iter->overlaps);
    for (p = iter->head; p != NULL; p = pnext) {
        pnext = p->next;
        mp_free(iter->mp, p);
    }
    mp_destroy(iter->mp);
    if (iter->b) bam_destroy1(iter->b);
    free(iter->plp);
    free(iter);
}

void bam_plp_constructor(bam_plp_t plp,
                         int (*func)(void *data, const bam1_t *b, bam_pileup_cd *cd)) {
    plp->plp_construct = func;
}

void bam_plp_destructor(bam_plp_t plp,
                        int (*func)(void *data, const bam1_t *b, bam_pileup_cd *cd)) {
    plp->plp_destruct = func;
}

//---------------------------------
//---  Tweak overlapping reads
//---------------------------------

/**
 *  cigar_iref2iseq_set()  - find the first CMATCH setting the ref and the read index
 *  cigar_iref2iseq_next() - get the next CMATCH base
 *  @cigar:       pointer to current cigar block (rw)
 *  @cigar_max:   pointer just beyond the last cigar block
 *  @icig:        position within the current cigar block (rw)
 *  @iseq:        position in the sequence (rw)
 *  @iref:        position with respect to the beginning of the read (iref_pos - b->core.pos) (rw)
 *
 *  Returns BAM_CMATCH or -1 when there is no more cigar to process or the requested position is not covered.
 */
static inline int cigar_iref2iseq_set(uint32_t **cigar, uint32_t *cigar_max, int *icig, int *iseq, int *iref)
{
    int pos = *iref;
    if ( pos < 0 ) return -1;
    *icig = 0;
    *iseq = 0;
    *iref = 0;
    while ( *cigar<cigar_max )
    {
        int cig  = (**cigar) & BAM_CIGAR_MASK;
        int ncig = (**cigar) >> BAM_CIGAR_SHIFT;

        if ( cig==BAM_CSOFT_CLIP ) { (*cigar)++; *iseq += ncig; *icig = 0; continue; }
        if ( cig==BAM_CHARD_CLIP || cig==BAM_CPAD ) { (*cigar)++; *icig = 0; continue; }
        if ( cig==BAM_CMATCH || cig==BAM_CEQUAL || cig==BAM_CDIFF )
        {
            pos -= ncig;
            if ( pos < 0 ) { *icig = ncig + pos; *iseq += *icig; *iref += *icig; return BAM_CMATCH; }
            (*cigar)++; *iseq += ncig; *icig = 0; *iref += ncig;
            continue;
        }
        if ( cig==BAM_CINS ) { (*cigar)++; *iseq += ncig; *icig = 0; continue; }
        if ( cig==BAM_CDEL || cig==BAM_CREF_SKIP )
        {
            pos -= ncig;
            if ( pos<0 ) pos = 0;
            (*cigar)++; *icig = 0; *iref += ncig;
            continue;
        }
        hts_log_error("Unexpected cigar %d", cig);
        assert(0);
    }
    *iseq = -1;
    return -1;
}
static inline int cigar_iref2iseq_next(uint32_t **cigar, uint32_t *cigar_max, int *icig, int *iseq, int *iref)
{
    while ( *cigar < cigar_max )
    {
        int cig  = (**cigar) & BAM_CIGAR_MASK;
        int ncig = (**cigar) >> BAM_CIGAR_SHIFT;

        if ( cig==BAM_CMATCH || cig==BAM_CEQUAL || cig==BAM_CDIFF )
        {
            if ( *icig >= ncig - 1 ) { *icig = 0;  (*cigar)++; continue; }
            (*iseq)++; (*icig)++; (*iref)++;
            return BAM_CMATCH;
        }
        if ( cig==BAM_CDEL || cig==BAM_CREF_SKIP ) { (*cigar)++; (*iref) += ncig; *icig = 0; continue; }
        if ( cig==BAM_CINS ) { (*cigar)++; *iseq += ncig; *icig = 0; continue; }
        if ( cig==BAM_CSOFT_CLIP ) { (*cigar)++; *iseq += ncig; *icig = 0; continue; }
        if ( cig==BAM_CHARD_CLIP || cig==BAM_CPAD ) { (*cigar)++; *icig = 0; continue; }
        hts_log_error("Unexpected cigar %d", cig);
        assert(0);
    }
    *iseq = -1;
    *iref = -1;
    return -1;
}

static void tweak_overlap_quality(bam1_t *a, bam1_t *b)
{
    uint32_t *a_cigar = bam_get_cigar(a), *a_cigar_max = a_cigar + a->core.n_cigar;
    uint32_t *b_cigar = bam_get_cigar(b), *b_cigar_max = b_cigar + b->core.n_cigar;
    int a_icig = 0, a_iseq = 0;
    int b_icig = 0, b_iseq = 0;
    uint8_t *a_qual = bam_get_qual(a), *b_qual = bam_get_qual(b);
    uint8_t *a_seq  = bam_get_seq(a), *b_seq = bam_get_seq(b);

    int iref   = b->core.pos;
    int a_iref = iref - a->core.pos;
    int b_iref = iref - b->core.pos;
    int a_ret = cigar_iref2iseq_set(&a_cigar, a_cigar_max, &a_icig, &a_iseq, &a_iref);
    if ( a_ret<0 ) return;  // no overlap
    int b_ret = cigar_iref2iseq_set(&b_cigar, b_cigar_max, &b_icig, &b_iseq, &b_iref);
    if ( b_ret<0 ) return;  // no overlap

    #if DBG
        fprintf(stderr,"tweak %s  n_cigar=%d %d  .. %d-%d vs %d-%d\n", bam_get_qname(a), a->core.n_cigar, b->core.n_cigar,
            a->core.pos+1,a->core.pos+bam_cigar2rlen(a->core.n_cigar,bam_get_cigar(a)), b->core.pos+1, b->core.pos+bam_cigar2rlen(b->core.n_cigar,bam_get_cigar(b)));
    #endif

    while ( 1 )
    {
        // Increment reference position
        while ( a_iref>=0 && a_iref < iref - a->core.pos )
            a_ret = cigar_iref2iseq_next(&a_cigar, a_cigar_max, &a_icig, &a_iseq, &a_iref);
        if ( a_ret<0 ) break;   // done
        if ( iref < a_iref + a->core.pos ) iref = a_iref + a->core.pos;

        while ( b_iref>=0 && b_iref < iref - b->core.pos )
            b_ret = cigar_iref2iseq_next(&b_cigar, b_cigar_max, &b_icig, &b_iseq, &b_iref);
        if ( b_ret<0 ) break;   // done
        if ( iref < b_iref + b->core.pos ) iref = b_iref + b->core.pos;

        iref++;
        if ( a_iref+a->core.pos != b_iref+b->core.pos ) continue;   // only CMATCH positions, don't know what to do with indels

        if ( bam_seqi(a_seq,a_iseq) == bam_seqi(b_seq,b_iseq) )
        {
            #if DBG
                fprintf(stderr,"%c",seq_nt16_str[bam_seqi(a_seq,a_iseq)]);
            #endif
            // we are very confident about this base
            int qual = a_qual[a_iseq] + b_qual[b_iseq];
            a_qual[a_iseq] = qual>200 ? 200 : qual;
            b_qual[b_iseq] = 0;
        }
        else
        {
            if ( a_qual[a_iseq] >= b_qual[b_iseq] )
            {
                #if DBG
                    fprintf(stderr,"[%c/%c]",seq_nt16_str[bam_seqi(a_seq,a_iseq)],tolower_c(seq_nt16_str[bam_seqi(b_seq,b_iseq)]));
                #endif
                a_qual[a_iseq] = 0.8 * a_qual[a_iseq];  // not so confident about a_qual anymore given the mismatch
                b_qual[b_iseq] = 0;
            }
            else
            {
                #if DBG
                    fprintf(stderr,"[%c/%c]",tolower_c(seq_nt16_str[bam_seqi(a_seq,a_iseq)]),seq_nt16_str[bam_seqi(b_seq,b_iseq)]);
                #endif
                b_qual[b_iseq] = 0.8 * b_qual[b_iseq];
                a_qual[a_iseq] = 0;
            }
        }
    }
    #if DBG
        fprintf(stderr,"\n");
    #endif
}

// Fix overlapping reads. Simple soft-clipping did not give good results.
// Lowering qualities of unwanted bases is more selective and works better.
//
static void overlap_push(bam_plp_t iter, lbnode_t *node)
{
    if ( !iter->overlaps ) return;

    // mapped mates and paired reads only
    if ( node->b.core.flag&BAM_FMUNMAP || !(node->b.core.flag&BAM_FPROPER_PAIR) ) return;

    // no overlap possible, unless some wild cigar
    if ( abs(node->b.core.isize) >= 2*node->b.core.l_qseq ) return;

    khiter_t kitr = kh_get(olap_hash, iter->overlaps, bam_get_qname(&node->b));
    if ( kitr==kh_end(iter->overlaps) )
    {
        int ret;
        kitr = kh_put(olap_hash, iter->overlaps, bam_get_qname(&node->b), &ret);
        kh_value(iter->overlaps, kitr) = node;
    }
    else
    {
        lbnode_t *a = kh_value(iter->overlaps, kitr);
        tweak_overlap_quality(&a->b, &node->b);
        kh_del(olap_hash, iter->overlaps, kitr);
        assert(a->end-1 == a->s.end);
        a->end = bam_endpos(&a->b);
        a->s.end = a->end - 1;
    }
}

static void overlap_remove(bam_plp_t iter, const bam1_t *b)
{
    if ( !iter->overlaps ) return;

    khiter_t kitr;
    if ( b )
    {
        kitr = kh_get(olap_hash, iter->overlaps, bam_get_qname(b));
        if ( kitr!=kh_end(iter->overlaps) )
            kh_del(olap_hash, iter->overlaps, kitr);
    }
    else
    {
        // remove all
        for (kitr = kh_begin(iter->overlaps); kitr<kh_end(iter->overlaps); kitr++)
            if ( kh_exist(iter->overlaps, kitr) ) kh_del(olap_hash, iter->overlaps, kitr);
    }
}



// Prepares next pileup position in bam records collected by bam_plp_auto -> user func -> bam_plp_push. Returns
// pointer to the piled records if next position is ready or NULL if there is not enough records in the
// buffer yet (the current position is still the maximum position across all buffered reads).
const bam_pileup1_t *bam_plp_next(bam_plp_t iter, int *_tid, int *_pos, int *_n_plp)
{
    if (iter->error) { *_n_plp = -1; return NULL; }
    *_n_plp = 0;
    if (iter->is_eof && iter->head == iter->tail) return NULL;
    while (iter->is_eof || iter->max_tid > iter->tid || (iter->max_tid == iter->tid && iter->max_pos > iter->pos)) {
        int n_plp = 0;
        // write iter->plp at iter->pos
        lbnode_t **pptr = &iter->head;
        while (*pptr != iter->tail) {
            lbnode_t *p = *pptr;
            if (p->b.core.tid < iter->tid || (p->b.core.tid == iter->tid && p->end <= iter->pos)) { // then remove
                overlap_remove(iter, &p->b);
                if (iter->plp_destruct)
                    iter->plp_destruct(iter->data, &p->b, &p->cd);
                *pptr = p->next; mp_free(iter->mp, p);
            }
            else {
                if (p->b.core.tid == iter->tid && p->beg <= iter->pos) { // here: p->end > pos; then add to pileup
                    if (n_plp == iter->max_plp) { // then double the capacity
                        iter->max_plp = iter->max_plp? iter->max_plp<<1 : 256;
                        iter->plp = (bam_pileup1_t*)realloc(iter->plp, sizeof(bam_pileup1_t) * iter->max_plp);
                    }
                    iter->plp[n_plp].b = &p->b;
                    iter->plp[n_plp].cd = p->cd;
                    if (resolve_cigar2(iter->plp + n_plp, iter->pos, &p->s)) ++n_plp; // actually always true...
                }
                pptr = &(*pptr)->next;
            }
        }
        *_n_plp = n_plp; *_tid = iter->tid; *_pos = iter->pos;
        // update iter->tid and iter->pos
        if (iter->head != iter->tail) {
            if (iter->tid > iter->head->b.core.tid) {
                hts_log_error("Unsorted input. Pileup aborts");
                iter->error = 1;
                *_n_plp = -1;
                return NULL;
            }
        }
        if (iter->tid < iter->head->b.core.tid) { // come to a new reference sequence
            iter->tid = iter->head->b.core.tid; iter->pos = iter->head->beg; // jump to the next reference
        } else if (iter->pos < iter->head->beg) { // here: tid == head->b.core.tid
            iter->pos = iter->head->beg; // jump to the next position
        } else ++iter->pos; // scan contiguously
        // return
        if (n_plp) return iter->plp;
        if (iter->is_eof && iter->head == iter->tail) break;
    }
    return NULL;
}

int bam_plp_push(bam_plp_t iter, const bam1_t *b)
{
    if (iter->error) return -1;
    if (b) {
        if (b->core.tid < 0) { overlap_remove(iter, b); return 0; }
        // Skip only unmapped reads here, any additional filtering must be done in iter->func
        if (b->core.flag & BAM_FUNMAP) { overlap_remove(iter, b); return 0; }
        if (iter->tid == b->core.tid && iter->pos == b->core.pos && iter->mp->cnt > iter->maxcnt)
        {
            overlap_remove(iter, b);
            return 0;
        }
        bam_copy1(&iter->tail->b, b);
        overlap_push(iter, iter->tail);
#ifndef BAM_NO_ID
        iter->tail->b.id = iter->id++;
#endif
        iter->tail->beg = b->core.pos;
        iter->tail->end = bam_endpos(b);
        iter->tail->s = g_cstate_null; iter->tail->s.end = iter->tail->end - 1; // initialize cstate_t
        if (b->core.tid < iter->max_tid) {
            hts_log_error("The input is not sorted (chromosomes out of order)");
            iter->error = 1;
            return -1;
        }
        if ((b->core.tid == iter->max_tid) && (iter->tail->beg < iter->max_pos)) {
            hts_log_error("The input is not sorted (reads out of order)");
            iter->error = 1;
            return -1;
        }
        iter->max_tid = b->core.tid; iter->max_pos = iter->tail->beg;
        if (iter->tail->end > iter->pos || iter->tail->b.core.tid > iter->tid) {
            if (iter->plp_construct)
                iter->plp_construct(iter->data, b, &iter->tail->cd);
            iter->tail->next = mp_alloc(iter->mp);
            iter->tail = iter->tail->next;
        }
    } else iter->is_eof = 1;
    return 0;
}

const bam_pileup1_t *bam_plp_auto(bam_plp_t iter, int *_tid, int *_pos, int *_n_plp)
{
    const bam_pileup1_t *plp;
    if (iter->func == 0 || iter->error) { *_n_plp = -1; return 0; }
    if ((plp = bam_plp_next(iter, _tid, _pos, _n_plp)) != 0) return plp;
    else { // no pileup line can be obtained; read alignments
        *_n_plp = 0;
        if (iter->is_eof) return 0;
        int ret;
        while ( (ret=iter->func(iter->data, iter->b)) >= 0) {
            if (bam_plp_push(iter, iter->b) < 0) {
                *_n_plp = -1;
                return 0;
            }
            if ((plp = bam_plp_next(iter, _tid, _pos, _n_plp)) != 0) return plp;
            // otherwise no pileup line can be returned; read the next alignment.
        }
        if ( ret < -1 ) { iter->error = ret; *_n_plp = -1; return 0; }
        bam_plp_push(iter, 0);
        if ((plp = bam_plp_next(iter, _tid, _pos, _n_plp)) != 0) return plp;
        return 0;
    }
}

void bam_plp_reset(bam_plp_t iter)
{
    overlap_remove(iter, NULL);
    iter->max_tid = iter->max_pos = -1;
    iter->tid = iter->pos = 0;
    iter->is_eof = 0;
    while (iter->head != iter->tail) {
        lbnode_t *p = iter->head;
        iter->head = p->next;
        mp_free(iter->mp, p);
    }
}

void bam_plp_set_maxcnt(bam_plp_t iter, int maxcnt)
{
    iter->maxcnt = maxcnt;
}

/************************
 *** Mpileup iterator ***
 ************************/

struct __bam_mplp_t {
    int n;
    uint64_t min, *pos;
    bam_plp_t *iter;
    int *n_plp;
    const bam_pileup1_t **plp;
};

bam_mplp_t bam_mplp_init(int n, bam_plp_auto_f func, void **data)
{
    int i;
    bam_mplp_t iter;
    iter = (bam_mplp_t)calloc(1, sizeof(struct __bam_mplp_t));
    iter->pos = (uint64_t*)calloc(n, sizeof(uint64_t));
    iter->n_plp = (int*)calloc(n, sizeof(int));
    iter->plp = (const bam_pileup1_t**)calloc(n, sizeof(bam_pileup1_t*));
    iter->iter = (bam_plp_t*)calloc(n, sizeof(bam_plp_t));
    iter->n = n;
    iter->min = (uint64_t)-1;
    for (i = 0; i < n; ++i) {
        iter->iter[i] = bam_plp_init(func, data[i]);
        iter->pos[i] = iter->min;
    }
    return iter;
}

void bam_mplp_init_overlaps(bam_mplp_t iter)
{
    int i;
    for (i = 0; i < iter->n; ++i)
        bam_plp_init_overlaps(iter->iter[i]);
}

void bam_mplp_set_maxcnt(bam_mplp_t iter, int maxcnt)
{
    int i;
    for (i = 0; i < iter->n; ++i)
        iter->iter[i]->maxcnt = maxcnt;
}

void bam_mplp_destroy(bam_mplp_t iter)
{
    int i;
    for (i = 0; i < iter->n; ++i) bam_plp_destroy(iter->iter[i]);
    free(iter->iter); free(iter->pos); free(iter->n_plp); free(iter->plp);
    free(iter);
}

int bam_mplp_auto(bam_mplp_t iter, int *_tid, int *_pos, int *n_plp, const bam_pileup1_t **plp)
{
    int i, ret = 0;
    uint64_t new_min = (uint64_t)-1;
    for (i = 0; i < iter->n; ++i) {
        if (iter->pos[i] == iter->min) {
            int tid, pos;
            iter->plp[i] = bam_plp_auto(iter->iter[i], &tid, &pos, &iter->n_plp[i]);
            if ( iter->iter[i]->error ) return -1;
            iter->pos[i] = iter->plp[i] ? (uint64_t)tid<<32 | pos : 0;
        }
        if (iter->plp[i] && iter->pos[i] < new_min) new_min = iter->pos[i];
    }
    iter->min = new_min;
    if (new_min == (uint64_t)-1) return 0;
    *_tid = new_min>>32; *_pos = (uint32_t)new_min;
    for (i = 0; i < iter->n; ++i) {
        if (iter->pos[i] == iter->min) { // FIXME: valgrind reports "uninitialised value(s) at this line"
            n_plp[i] = iter->n_plp[i], plp[i] = iter->plp[i];
            ++ret;
        } else n_plp[i] = 0, plp[i] = 0;
    }
    return ret;
}

void bam_mplp_reset(bam_mplp_t iter)
{
    int i;
    iter->min = (uint64_t)-1;
    for (i = 0; i < iter->n; ++i) {
        bam_plp_reset(iter->iter[i]);
        iter->pos[i] = (uint64_t)-1;
        iter->n_plp[i] = 0;
        iter->plp[i] = NULL;
    }
}

void bam_mplp_constructor(bam_mplp_t iter,
                          int (*func)(void *arg, const bam1_t *b, bam_pileup_cd *cd)) {
    int i;
    for (i = 0; i < iter->n; ++i)
        bam_plp_constructor(iter->iter[i], func);
}

void bam_mplp_destructor(bam_mplp_t iter,
                         int (*func)(void *arg, const bam1_t *b, bam_pileup_cd *cd)) {
    int i;
    for (i = 0; i < iter->n; ++i)
        bam_plp_destructor(iter->iter[i], func);
}

#endif // ~!defined(BAM_NO_PILEUP)

//						↓↓↓VULNERABLE LINES↓↓↓

// 1674,41;1674,47

// 1675,42;1675,48

// 1676,41;1676,47

// 1697,4;1697,10

// 1725,8;1725,14

// 1729,8;1729,14

// 1738,4;1738,10

