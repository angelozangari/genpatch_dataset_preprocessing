/*
 * Windows Television (WTV) demuxer
 * Copyright (c) 2010-2011 Peter Ross <pross@xvid.org>
 *
 * This file is part of Libav.
 *
 * Libav is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Libav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Libav; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * Windows Television (WTV) demuxer
 * @author Peter Ross <pross@xvid.org>
 */

#include <inttypes.h>

#include "libavutil/channel_layout.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/intfloat.h"
#include "libavutil/dict.h"
#include "avformat.h"
#include "internal.h"
#include "riff.h"
#include "asf.h"
#include "mpegts.h"

/* Macros for formating GUIDs */
#define PRI_PRETTY_GUID \
    "%08"PRIx32"-%04"PRIx16"-%04"PRIx16"-%02x%02x%02x%02x%02x%02x%02x%02x"
#define ARG_PRETTY_GUID(g) \
    AV_RL32(g),AV_RL16(g+4),AV_RL16(g+6),g[8],g[9],g[10],g[11],g[12],g[13],g[14],g[15]
#define LEN_PRETTY_GUID 34

/*
 *
 * File system routines
 *
 */

#define WTV_SECTOR_BITS    12
#define WTV_SECTOR_SIZE    (1 << WTV_SECTOR_BITS)
#define WTV_BIGSECTOR_BITS 18

#define SHIFT_SECTOR_BITS(a) ((int64_t)(a) << WTV_SECTOR_BITS)

typedef struct {
    AVIOContext *pb_filesystem;  /** file system (AVFormatContext->pb) */

    int sector_bits;     /** sector shift bits; used to convert sector number into pb_filesystem offset */
    uint32_t *sectors;   /** file allocation table */
    int nb_sectors;      /** number of sectors */

    int error;
    int64_t position;
    int64_t length;
} WtvFile;

static int64_t seek_by_sector(AVIOContext *pb, int64_t sector, int64_t offset)
{
    return avio_seek(pb, SHIFT_SECTOR_BITS(sector) + offset, SEEK_SET);
}

/**
 * @return bytes read, 0 on end of file, or <0 on error
 */
static int wtvfile_read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    WtvFile *wf = opaque;
    AVIOContext *pb = wf->pb_filesystem;
    int nread = 0;

    if (wf->error || pb->error)
        return -1;
    if (wf->position >= wf->length || pb->eof_reached)
        return 0;

    buf_size = FFMIN(buf_size, wf->length - wf->position);
    while(nread < buf_size) {
        int n;
        int remaining_in_sector = (1 << wf->sector_bits) - (wf->position & ((1 << wf->sector_bits) - 1));
        int read_request        = FFMIN(buf_size - nread, remaining_in_sector);

        n = avio_read(pb, buf, read_request);
        if (n <= 0)
            break;
        nread += n;
        buf += n;
        wf->position += n;
        if (n == remaining_in_sector) {
            int i = wf->position >> wf->sector_bits;
            if (i >= wf->nb_sectors ||
                (wf->sectors[i] != wf->sectors[i - 1] + (1 << (wf->sector_bits - WTV_SECTOR_BITS)) &&
                seek_by_sector(pb, wf->sectors[i], 0) < 0)) {
                wf->error = 1;
                break;
            }
        }
    }
    return nread;
}

/**
 * @return position (or file length)
 */
static int64_t wtvfile_seek(void *opaque, int64_t offset, int whence)
{
    WtvFile *wf = opaque;
    AVIOContext *pb = wf->pb_filesystem;

    if (whence == AVSEEK_SIZE)
        return wf->length;
    else if (whence == SEEK_CUR)
        offset = wf->position + offset;
    else if (whence == SEEK_END)
        offset = wf->length;

    wf->error = offset < 0 || offset >= wf->length ||
                seek_by_sector(pb, wf->sectors[offset >> wf->sector_bits],
                               offset & ((1 << wf->sector_bits) - 1)) < 0;
    wf->position = offset;
    return offset;
}

/**
 * read non-zero integers (le32) from input stream
 * @param pb
 * @param[out] data destination
 * @param     count maximum number of integers to read
 * @return    total number of integers read
 */
static int read_ints(AVIOContext *pb, uint32_t *data, int count)
{
    int i, total = 0;
    for (i = 0; i < count; i++) {
        if ((data[total] = avio_rl32(pb)))
           total++;
    }
    return total;
}

/**
 * Open file
 * @param first_sector  First sector
 * @param length        Length of file (bytes)
 * @param depth         File allocation table depth
 * @return NULL on error
 */
static AVIOContext * wtvfile_open_sector(int first_sector, uint64_t length, int depth, AVFormatContext *s)
{
    AVIOContext *pb;
    WtvFile *wf;
    uint8_t *buffer;

    if (seek_by_sector(s->pb, first_sector, 0) < 0)
        return NULL;

    wf = av_mallocz(sizeof(WtvFile));
    if (!wf)
        return NULL;

    if (depth == 0) {
        wf->sectors = av_malloc(sizeof(uint32_t));
        if (!wf->sectors) {
            av_free(wf);
            return NULL;
        }
        wf->sectors[0]  = first_sector;
        wf->nb_sectors  = 1;
        wf->sector_bits = WTV_SECTOR_BITS;
    } else if (depth == 1) {
        wf->sectors = av_malloc(WTV_SECTOR_SIZE);
        if (!wf->sectors) {
            av_free(wf);
            return NULL;
        }
        wf->nb_sectors  = read_ints(s->pb, wf->sectors, WTV_SECTOR_SIZE / 4);
        wf->sector_bits = length & (1ULL<<63) ? WTV_SECTOR_BITS : WTV_BIGSECTOR_BITS;
    } else if (depth == 2) {
        uint32_t sectors1[WTV_SECTOR_SIZE / 4];
        int nb_sectors1 = read_ints(s->pb, sectors1, WTV_SECTOR_SIZE / 4);
        int i;

        wf->sectors = av_malloc(SHIFT_SECTOR_BITS(nb_sectors1));
        if (!wf->sectors) {
            av_free(wf);
            return NULL;
        }
        wf->nb_sectors = 0;
        for (i = 0; i < nb_sectors1; i++) {
            if (seek_by_sector(s->pb, sectors1[i], 0) < 0)
                break;
            wf->nb_sectors += read_ints(s->pb, wf->sectors + i * WTV_SECTOR_SIZE / 4, WTV_SECTOR_SIZE / 4);
        }
        wf->sector_bits = length & (1ULL<<63) ? WTV_SECTOR_BITS : WTV_BIGSECTOR_BITS;
    } else {
        av_log(s, AV_LOG_ERROR, "unsupported file allocation table depth (0x%x)\n", depth);
        av_free(wf);
        return NULL;
    }

    if (!wf->nb_sectors) {
        av_free(wf->sectors);
        av_free(wf);
        return NULL;
    }

    /* check length */
    length &= 0xFFFFFFFFFFFF;
    if (length > ((int64_t)wf->nb_sectors << wf->sector_bits)) {
        av_log(s, AV_LOG_WARNING, "reported file length (0x%"PRIx64") exceeds number of available sectors (0x%"PRIx64")\n", length, (int64_t)wf->nb_sectors << wf->sector_bits);
        length = (int64_t)wf->nb_sectors <<  wf->sector_bits;
    }
    wf->length = length;

    /* seek to initial sector */
    wf->position = 0;
    if (seek_by_sector(s->pb, wf->sectors[0], 0) < 0) {
        av_free(wf->sectors);
        av_free(wf);
        return NULL;
    }

    wf->pb_filesystem = s->pb;
    buffer = av_malloc(1 << wf->sector_bits);
    if (!buffer) {
        av_free(wf->sectors);
        av_free(wf);
        return NULL;
    }

    pb = avio_alloc_context(buffer, 1 << wf->sector_bits, 0, wf,
                           wtvfile_read_packet, NULL, wtvfile_seek);
    if (!pb) {
        av_free(buffer);
        av_free(wf->sectors);
        av_free(wf);
    }
    return pb;
}

static const ff_asf_guid dir_entry_guid =
    {0x92,0xB7,0x74,0x91,0x59,0x70,0x70,0x44,0x88,0xDF,0x06,0x3B,0x82,0xCC,0x21,0x3D};

/**
 * Open file using filename
 * @param[in]  buf       directory buffer
 * @param      buf_size  directory buffer size
 * @param[in]  filename
 * @param      filename_size size of filename
 * @return NULL on error
 */
static AVIOContext * wtvfile_open2(AVFormatContext *s, const uint8_t *buf, int buf_size, const uint8_t *filename, int filename_size)
{
    const uint8_t *buf_end = buf + buf_size;

    while(buf + 48 <= buf_end) {
        int dir_length, name_size, first_sector, depth;
        uint64_t file_length;
        const uint8_t *name;
        if (ff_guidcmp(buf, dir_entry_guid)) {
            av_log(s, AV_LOG_ERROR, "unknown guid "FF_PRI_GUID", expected dir_entry_guid; "
                   "remaining directory entries ignored\n", FF_ARG_GUID(buf));
            break;
        }
        dir_length  = AV_RL16(buf + 16);
        file_length = AV_RL64(buf + 24);
        name_size   = 2 * AV_RL32(buf + 32);
        if (name_size < 0) {
            av_log(s, AV_LOG_ERROR,
                   "bad filename length, remaining directory entries ignored\n");
            break;
        }
        if (48 + name_size > buf_end - buf) {
            av_log(s, AV_LOG_ERROR, "filename exceeds buffer size; remaining directory entries ignored\n");
            break;
        }
        first_sector = AV_RL32(buf + 40 + name_size);
        depth        = AV_RL32(buf + 44 + name_size);

        /* compare file name; test optional null terminator */
        name = buf + 40;
        if (name_size >= filename_size &&
            !memcmp(name, filename, filename_size) &&
            (name_size < filename_size + 2 || !AV_RN16(name + filename_size)))
            return wtvfile_open_sector(first_sector, file_length, depth, s);

        buf += dir_length;
    }
    return 0;
}

#define wtvfile_open(s, buf, buf_size, filename) \
    wtvfile_open2(s, buf, buf_size, filename, sizeof(filename))

/**
 * Close file opened with wtvfile_open_sector(), or wtv_open()
 */
static void wtvfile_close(AVIOContext *pb)
{
    WtvFile *wf = pb->opaque;
    av_free(wf->sectors);
    av_free(wf);
    av_free(pb->buffer);
    av_free(pb);
}

/*
 *
 * Main demuxer
 *
 */

typedef struct {
    int seen_data;
} WtvStream;

typedef struct {
    AVIOContext *pb;       /** timeline file */
    int64_t epoch;
    int64_t pts;             /** pts for next data chunk */
    int64_t last_valid_pts;  /** latest valid pts, used for interative seeking */

    /* maintain private seek index, as the AVIndexEntry->pos is relative to the
       start of the 'timeline' file, not the file system (AVFormatContext->pb) */
    AVIndexEntry *index_entries;
    int nb_index_entries;
    unsigned int index_entries_allocated_size;
} WtvContext;

/* WTV GUIDs */
static const ff_asf_guid wtv_guid =
    {0xB7,0xD8,0x00,0x20,0x37,0x49,0xDA,0x11,0xA6,0x4E,0x00,0x07,0xE9,0x5E,0xAD,0x8D};
static const ff_asf_guid metadata_guid =
    {0x5A,0xFE,0xD7,0x6D,0xC8,0x1D,0x8F,0x4A,0x99,0x22,0xFA,0xB1,0x1C,0x38,0x14,0x53};
static const ff_asf_guid timestamp_guid =
    {0x5B,0x05,0xE6,0x1B,0x97,0xA9,0x49,0x43,0x88,0x17,0x1A,0x65,0x5A,0x29,0x8A,0x97};
static const ff_asf_guid data_guid =
    {0x95,0xC3,0xD2,0xC2,0x7E,0x9A,0xDA,0x11,0x8B,0xF7,0x00,0x07,0xE9,0x5E,0xAD,0x8D};
static const ff_asf_guid stream_guid =
    {0xED,0xA4,0x13,0x23,0x2D,0xBF,0x4F,0x45,0xAD,0x8A,0xD9,0x5B,0xA7,0xF9,0x1F,0xEE};
static const ff_asf_guid stream2_guid =
    {0xA2,0xC3,0xD2,0xC2,0x7E,0x9A,0xDA,0x11,0x8B,0xF7,0x00,0x07,0xE9,0x5E,0xAD,0x8D};
static const ff_asf_guid EVENTID_SubtitleSpanningEvent =
    {0x48,0xC0,0xCE,0x5D,0xB9,0xD0,0x63,0x41,0x87,0x2C,0x4F,0x32,0x22,0x3B,0xE8,0x8A};
static const ff_asf_guid EVENTID_LanguageSpanningEvent =
    {0x6D,0x66,0x92,0xE2,0x02,0x9C,0x8D,0x44,0xAA,0x8D,0x78,0x1A,0x93,0xFD,0xC3,0x95};
static const ff_asf_guid EVENTID_AudioDescriptorSpanningEvent =
    {0x1C,0xD4,0x7B,0x10,0xDA,0xA6,0x91,0x46,0x83,0x69,0x11,0xB2,0xCD,0xAA,0x28,0x8E};
static const ff_asf_guid EVENTID_CtxADescriptorSpanningEvent =
    {0xE6,0xA2,0xB4,0x3A,0x47,0x42,0x34,0x4B,0x89,0x6C,0x30,0xAF,0xA5,0xD2,0x1C,0x24};
static const ff_asf_guid EVENTID_CSDescriptorSpanningEvent =
    {0xD9,0x79,0xE7,0xEf,0xF0,0x97,0x86,0x47,0x80,0x0D,0x95,0xCF,0x50,0x5D,0xDC,0x66};
static const ff_asf_guid EVENTID_DVBScramblingControlSpanningEvent =
    {0xC4,0xE1,0xD4,0x4B,0xA1,0x90,0x09,0x41,0x82,0x36,0x27,0xF0,0x0E,0x7D,0xCC,0x5B};
static const ff_asf_guid EVENTID_StreamIDSpanningEvent =
    {0x68,0xAB,0xF1,0xCA,0x53,0xE1,0x41,0x4D,0xA6,0xB3,0xA7,0xC9,0x98,0xDB,0x75,0xEE};
static const ff_asf_guid EVENTID_TeletextSpanningEvent =
    {0x50,0xD9,0x99,0x95,0x33,0x5F,0x17,0x46,0xAF,0x7C,0x1E,0x54,0xB5,0x10,0xDA,0xA3};
static const ff_asf_guid EVENTID_AudioTypeSpanningEvent =
    {0xBE,0xBF,0x1C,0x50,0x49,0xB8,0xCE,0x42,0x9B,0xE9,0x3D,0xB8,0x69,0xFB,0x82,0xB3};

/* Windows media GUIDs */

/* Media types */
static const ff_asf_guid mediatype_audio =
    {'a','u','d','s',FF_MEDIASUBTYPE_BASE_GUID};
static const ff_asf_guid mediatype_video =
    {'v','i','d','s',FF_MEDIASUBTYPE_BASE_GUID};
static const ff_asf_guid mediasubtype_mpeg1payload =
    {0x81,0xEB,0x36,0xE4,0x4F,0x52,0xCE,0x11,0x9F,0x53,0x00,0x20,0xAF,0x0B,0xA7,0x70};
static const ff_asf_guid mediatype_mpeg2_sections =
    {0x6C,0x17,0x5F,0x45,0x06,0x4B,0xCE,0x47,0x9A,0xEF,0x8C,0xAE,0xF7,0x3D,0xF7,0xB5};
static const ff_asf_guid mediatype_mpeg2_pes =
    {0x20,0x80,0x6D,0xE0,0x46,0xDB,0xCF,0x11,0xB4,0xD1,0x00,0x80,0x5F,0x6C,0xBB,0xEA};
static const ff_asf_guid mediatype_mstvcaption =
    {0x89,0x8A,0x8B,0xB8,0x49,0xB0,0x80,0x4C,0xAD,0xCF,0x58,0x98,0x98,0x5E,0x22,0xC1};

/* Media subtypes */
static const ff_asf_guid mediasubtype_cpfilters_processed =
    {0x28,0xBD,0xAD,0x46,0xD0,0x6F,0x96,0x47,0x93,0xB2,0x15,0x5C,0x51,0xDC,0x04,0x8D};
static const ff_asf_guid mediasubtype_dvb_subtitle =
    {0xC3,0xCB,0xFF,0x34,0xB3,0xD5,0x71,0x41,0x90,0x02,0xD4,0xC6,0x03,0x01,0x69,0x7F};
static const ff_asf_guid mediasubtype_teletext =
    {0xE3,0x76,0x2A,0xF7,0x0A,0xEB,0xD0,0x11,0xAC,0xE4,0x00,0x00,0xC0,0xCC,0x16,0xBA};
static const ff_asf_guid mediasubtype_dtvccdata =
    {0xAA,0xDD,0x2A,0xF5,0xF0,0x36,0xF5,0x43,0x95,0xEA,0x6D,0x86,0x64,0x84,0x26,0x2A};
static const ff_asf_guid mediasubtype_mpeg2_sections =
    {0x79,0x85,0x9F,0x4A,0xF8,0x6B,0x92,0x43,0x8A,0x6D,0xD2,0xDD,0x09,0xFA,0x78,0x61};

/* Formats */
static const ff_asf_guid format_cpfilters_processed =
    {0x6F,0xB3,0x39,0x67,0x5F,0x1D,0xC2,0x4A,0x81,0x92,0x28,0xBB,0x0E,0x73,0xD1,0x6A};
static const ff_asf_guid format_waveformatex =
    {0x81,0x9F,0x58,0x05,0x56,0xC3,0xCE,0x11,0xBF,0x01,0x00,0xAA,0x00,0x55,0x59,0x5A};
static const ff_asf_guid format_videoinfo2 =
    {0xA0,0x76,0x2A,0xF7,0x0A,0xEB,0xD0,0x11,0xAC,0xE4,0x00,0x00,0xC0,0xCC,0x16,0xBA};
static const ff_asf_guid format_mpeg2_video =
    {0xE3,0x80,0x6D,0xE0,0x46,0xDB,0xCF,0x11,0xB4,0xD1,0x00,0x80,0x5F,0x6C,0xBB,0xEA};
static const ff_asf_guid format_none =
    {0xD6,0x17,0x64,0x0F,0x18,0xC3,0xD0,0x11,0xA4,0x3F,0x00,0xA0,0xC9,0x22,0x31,0x96};

static const AVCodecGuid video_guids[] = {
    {AV_CODEC_ID_MPEG2VIDEO, {0x26,0x80,0x6D,0xE0,0x46,0xDB,0xCF,0x11,0xB4,0xD1,0x00,0x80,0x5F,0x6C,0xBB,0xEA}},
    {AV_CODEC_ID_NONE}
};

static int read_probe(AVProbeData *p)
{
    return ff_guidcmp(p->buf, wtv_guid) ? 0 : AVPROBE_SCORE_MAX;
}

/**
 * Convert win32 FILETIME to ISO-8601 string
 */
static void filetime_to_iso8601(char *buf, int buf_size, int64_t value)
{
    time_t t = (value / 10000000LL) - 11644473600LL;
    struct tm *tm = gmtime(&t);
    if (tm)
        strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", gmtime(&t));
    else
        buf[0] = '\0';
}

/**
 * Convert crazy time (100ns since 1 Jan 0001) to ISO-8601 string
 */
static void crazytime_to_iso8601(char *buf, int buf_size, int64_t value)
{
    time_t t = (value / 10000000LL) - 719162LL*86400LL;
    struct tm *tm = gmtime(&t);
    if (tm)
        strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", gmtime(&t));
    else
        buf[0] = '\0';
}

/**
 * Convert OLE DATE to ISO-8601 string
 */
static void oledate_to_iso8601(char *buf, int buf_size, int64_t value)
{
    time_t t = 631112400LL + 86400*av_int2double(value);
    struct tm *tm = gmtime(&t);
    if (tm)
        strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", gmtime(&t));
    else
        buf[0] = '\0';
}

static void get_attachment(AVFormatContext *s, AVIOContext *pb, int length)
{
    char mime[1024];
    char description[1024];
    unsigned int filesize;
    AVStream *st;
    int64_t pos = avio_tell(pb);

    avio_get_str16le(pb, INT_MAX, mime, sizeof(mime));
    if (strcmp(mime, "image/jpeg"))
        goto done;

    avio_r8(pb);
    avio_get_str16le(pb, INT_MAX, description, sizeof(description));
    filesize = avio_rl32(pb);
    if (!filesize)
        goto done;

    st = avformat_new_stream(s, NULL);
    if (!st)
        goto done;
    av_dict_set(&st->metadata, "title", description, 0);
    st->codec->codec_id   = AV_CODEC_ID_MJPEG;
    st->codec->codec_type = AVMEDIA_TYPE_ATTACHMENT;
    st->codec->extradata  = av_mallocz(filesize);
    st->id = -1;
    if (!st->codec->extradata)
        goto done;
    st->codec->extradata_size = filesize;
    avio_read(pb, st->codec->extradata, filesize);
done:
    avio_seek(pb, pos + length, SEEK_SET);
}

static void get_tag(AVFormatContext *s, AVIOContext *pb, const char *key, int type, int length)
{
    int buf_size = FFMAX(2*length, LEN_PRETTY_GUID) + 1;
    char *buf = av_malloc(buf_size);
    if (!buf)
        return;

    if (type == 0 && length == 4) {
        snprintf(buf, buf_size, "%u", avio_rl32(pb));
    } else if (type == 1) {
        avio_get_str16le(pb, length, buf, buf_size);
        if (!strlen(buf)) {
           av_free(buf);
           return;
        }
    } else if (type == 3 && length == 4) {
        strcpy(buf, avio_rl32(pb) ? "true" : "false");
    } else if (type == 4 && length == 8) {
        int64_t num = avio_rl64(pb);
        if (!strcmp(key, "WM/EncodingTime") ||
            !strcmp(key, "WM/MediaOriginalBroadcastDateTime"))
            filetime_to_iso8601(buf, buf_size, num);
        else if (!strcmp(key, "WM/WMRVEncodeTime") ||
                 !strcmp(key, "WM/WMRVEndTime"))
            crazytime_to_iso8601(buf, buf_size, num);
        else if (!strcmp(key, "WM/WMRVExpirationDate"))
            oledate_to_iso8601(buf, buf_size, num);
        else if (!strcmp(key, "WM/WMRVBitrate"))
            snprintf(buf, buf_size, "%f", av_int2double(num));
        else
            snprintf(buf, buf_size, "%"PRIi64, num);
    } else if (type == 5 && length == 2) {
        snprintf(buf, buf_size, "%u", avio_rl16(pb));
    } else if (type == 6 && length == 16) {
        ff_asf_guid guid;
        avio_read(pb, guid, 16);
        snprintf(buf, buf_size, PRI_PRETTY_GUID, ARG_PRETTY_GUID(guid));
    } else if (type == 2 && !strcmp(key, "WM/Picture")) {
        get_attachment(s, pb, length);
        av_freep(&buf);
        return;
    } else {
        av_freep(&buf);
        av_log(s, AV_LOG_WARNING, "unsupported metadata entry; key:%s, type:%d, length:0x%x\n", key, type, length);
        avio_skip(pb, length);
        return;
    }

    av_dict_set(&s->metadata, key, buf, 0);
    av_freep(&buf);
}

/**
 * Parse metadata entries
 */
static void parse_legacy_attrib(AVFormatContext *s, AVIOContext *pb)
{
    ff_asf_guid guid;
    int length, type;
    while(!pb->eof_reached) {
        char key[1024];
        ff_get_guid(pb, &guid);
        type   = avio_rl32(pb);
        length = avio_rl32(pb);
        if (!length)
            break;
        if (ff_guidcmp(&guid, metadata_guid)) {
            av_log(s, AV_LOG_WARNING, "unknown guid "FF_PRI_GUID", expected metadata_guid; "
                   "remaining metadata entries ignored\n", FF_ARG_GUID(guid));
            break;
        }
        avio_get_str16le(pb, INT_MAX, key, sizeof(key));
        get_tag(s, pb, key, type, length);
    }

    ff_metadata_conv(&s->metadata, NULL, ff_asf_metadata_conv);
}

/**
 * parse VIDEOINFOHEADER2 structure
 * @return bytes consumed
 */
static int parse_videoinfoheader2(AVFormatContext *s, AVStream *st)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;

    avio_skip(pb, 72);  // picture aspect ratio is unreliable
    ff_get_bmp_header(pb, st);

    return 72 + 40;
}

/**
 * Parse MPEG1WAVEFORMATEX extradata structure
 */
static void parse_mpeg1waveformatex(AVStream *st)
{
    /* fwHeadLayer */
    switch (AV_RL16(st->codec->extradata)) {
    case 0x0001 : st->codec->codec_id = AV_CODEC_ID_MP1; break;
    case 0x0002 : st->codec->codec_id = AV_CODEC_ID_MP2; break;
    case 0x0004 : st->codec->codec_id = AV_CODEC_ID_MP3; break;
    }

    st->codec->bit_rate = AV_RL32(st->codec->extradata + 2); /* dwHeadBitrate */

    /* dwHeadMode */
    switch (AV_RL16(st->codec->extradata + 6)) {
    case 1 :
    case 2 :
    case 4 : st->codec->channels       = 2;
             st->codec->channel_layout = AV_CH_LAYOUT_STEREO;
             break;
    case 8 : st->codec->channels       = 1;
             st->codec->channel_layout = AV_CH_LAYOUT_MONO;
             break;
    }
}

/**
 * Initialise stream
 * @param st Stream to initialise, or NULL to create and initialise new stream
 * @return NULL on error
 */
static AVStream * new_stream(AVFormatContext *s, AVStream *st, int sid, int codec_type)
{
    if (st) {
        if (st->codec->extradata) {
            av_freep(&st->codec->extradata);
            st->codec->extradata_size = 0;
        }
    } else {
        WtvStream *wst = av_mallocz(sizeof(WtvStream));
        if (!wst)
            return NULL;
        st = avformat_new_stream(s, NULL);
        if (!st)
            return NULL;
        st->id = sid;
        st->priv_data = wst;
    }
    st->codec->codec_type = codec_type;
    st->need_parsing      = AVSTREAM_PARSE_FULL;
    avpriv_set_pts_info(st, 64, 1, 10000000);
    return st;
}

/**
 * parse Media Type structure and populate stream
 * @param st         Stream, or NULL to create new stream
 * @param mediatype  Mediatype GUID
 * @param subtype    Subtype GUID
 * @param formattype Format GUID
 * @param size       Size of format buffer
 * @return NULL on error
 */
static AVStream * parse_media_type(AVFormatContext *s, AVStream *st, int sid,
                                   ff_asf_guid mediatype, ff_asf_guid subtype,
                                   ff_asf_guid formattype, int size)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;
    if (!ff_guidcmp(subtype, mediasubtype_cpfilters_processed) &&
        !ff_guidcmp(formattype, format_cpfilters_processed)) {
        ff_asf_guid actual_subtype;
        ff_asf_guid actual_formattype;

        if (size < 32) {
            av_log(s, AV_LOG_WARNING, "format buffer size underflow\n");
            avio_skip(pb, size);
            return NULL;
        }

        avio_skip(pb, size - 32);
        ff_get_guid(pb, &actual_subtype);
        ff_get_guid(pb, &actual_formattype);
        avio_seek(pb, -size, SEEK_CUR);

        st = parse_media_type(s, st, sid, mediatype, actual_subtype, actual_formattype, size - 32);
        avio_skip(pb, 32);
        return st;
    } else if (!ff_guidcmp(mediatype, mediatype_audio)) {
        st = new_stream(s, st, sid, AVMEDIA_TYPE_AUDIO);
        if (!st)
            return NULL;
        if (!ff_guidcmp(formattype, format_waveformatex)) {
            int ret = ff_get_wav_header(pb, st->codec, size);
            if (ret < 0)
                return NULL;
        } else {
            if (ff_guidcmp(formattype, format_none))
                av_log(s, AV_LOG_WARNING, "unknown formattype:"FF_PRI_GUID"\n", FF_ARG_GUID(formattype));
            avio_skip(pb, size);
        }

        if (!memcmp(subtype + 4, (const uint8_t[]){FF_MEDIASUBTYPE_BASE_GUID}, 12)) {
            st->codec->codec_id = ff_wav_codec_get_id(AV_RL32(subtype), st->codec->bits_per_coded_sample);
        } else if (!ff_guidcmp(subtype, mediasubtype_mpeg1payload)) {
            if (st->codec->extradata && st->codec->extradata_size >= 22)
                parse_mpeg1waveformatex(st);
            else
                av_log(s, AV_LOG_WARNING, "MPEG1WAVEFORMATEX underflow\n");
        } else {
            st->codec->codec_id = ff_codec_guid_get_id(ff_codec_wav_guids, subtype);
            if (st->codec->codec_id == AV_CODEC_ID_NONE)
                av_log(s, AV_LOG_WARNING, "unknown subtype:"FF_PRI_GUID"\n", FF_ARG_GUID(subtype));
        }
        return st;
    } else if (!ff_guidcmp(mediatype, mediatype_video)) {
        st = new_stream(s, st, sid, AVMEDIA_TYPE_VIDEO);
        if (!st)
            return NULL;
        if (!ff_guidcmp(formattype, format_videoinfo2)) {
            int consumed = parse_videoinfoheader2(s, st);
            avio_skip(pb, FFMAX(size - consumed, 0));
        } else if (!ff_guidcmp(formattype, format_mpeg2_video)) {
            int consumed = parse_videoinfoheader2(s, st);
            avio_skip(pb, FFMAX(size - consumed, 0));
        } else {
            if (ff_guidcmp(formattype, format_none))
                av_log(s, AV_LOG_WARNING, "unknown formattype:"FF_PRI_GUID"\n", FF_ARG_GUID(formattype));
            avio_skip(pb, size);
        }

        if (!memcmp(subtype + 4, (const uint8_t[]){FF_MEDIASUBTYPE_BASE_GUID}, 12)) {
            st->codec->codec_id = ff_codec_get_id(ff_codec_bmp_tags, AV_RL32(subtype));
        } else {
            st->codec->codec_id = ff_codec_guid_get_id(video_guids, subtype);
        }
        if (st->codec->codec_id == AV_CODEC_ID_NONE)
            av_log(s, AV_LOG_WARNING, "unknown subtype:"FF_PRI_GUID"\n", FF_ARG_GUID(subtype));
        return st;
    } else if (!ff_guidcmp(mediatype, mediatype_mpeg2_pes) &&
               !ff_guidcmp(subtype, mediasubtype_dvb_subtitle)) {
        st = new_stream(s, st, sid, AVMEDIA_TYPE_SUBTITLE);
        if (!st)
            return NULL;
        if (ff_guidcmp(formattype, format_none))
            av_log(s, AV_LOG_WARNING, "unknown formattype:"FF_PRI_GUID"\n", FF_ARG_GUID(formattype));
        avio_skip(pb, size);
        st->codec->codec_id = AV_CODEC_ID_DVB_SUBTITLE;
        return st;
    } else if (!ff_guidcmp(mediatype, mediatype_mstvcaption) &&
               (!ff_guidcmp(subtype, mediasubtype_teletext) || !ff_guidcmp(subtype, mediasubtype_dtvccdata))) {
        st = new_stream(s, st, sid, AVMEDIA_TYPE_SUBTITLE);
        if (!st)
            return NULL;
        if (ff_guidcmp(formattype, format_none))
            av_log(s, AV_LOG_WARNING, "unknown formattype:"FF_PRI_GUID"\n", FF_ARG_GUID(formattype));
        avio_skip(pb, size);
        st->codec->codec_id   = AV_CODEC_ID_DVB_TELETEXT;
        return st;
    } else if (!ff_guidcmp(mediatype, mediatype_mpeg2_sections) &&
               !ff_guidcmp(subtype, mediasubtype_mpeg2_sections)) {
        if (ff_guidcmp(formattype, format_none))
            av_log(s, AV_LOG_WARNING, "unknown formattype:"FF_PRI_GUID"\n", FF_ARG_GUID(formattype));
        avio_skip(pb, size);
        return NULL;
    }

    av_log(s, AV_LOG_WARNING, "unknown media type, mediatype:"FF_PRI_GUID
                              ", subtype:"FF_PRI_GUID", formattype:"FF_PRI_GUID"\n",
                              FF_ARG_GUID(mediatype), FF_ARG_GUID(subtype), FF_ARG_GUID(formattype));
    avio_skip(pb, size);
    return NULL;
}

enum {
    SEEK_TO_DATA = 0,
    SEEK_TO_PTS,
};

/**
 * Parse WTV chunks
 * @param mode SEEK_TO_DATA or SEEK_TO_PTS
 * @param seekts timestamp
 * @param[out] len_ptr Length of data chunk
 * @return stream index of data chunk, or <0 on error
 */
static int parse_chunks(AVFormatContext *s, int mode, int64_t seekts, int *len_ptr)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;
    while (!pb->eof_reached) {
        ff_asf_guid g;
        int len, sid, consumed;

        ff_get_guid(pb, &g);
        len = avio_rl32(pb);
        if (len < 32)
            break;
        sid = avio_rl32(pb) & 0x7FFF;
        avio_skip(pb, 8);
        consumed = 32;

        if (!ff_guidcmp(g, stream_guid)) {
            if (ff_find_stream_index(s, sid) < 0) {
                ff_asf_guid mediatype, subtype, formattype;
                int size;
                avio_skip(pb, 28);
                ff_get_guid(pb, &mediatype);
                ff_get_guid(pb, &subtype);
                avio_skip(pb, 12);
                ff_get_guid(pb, &formattype);
                size = avio_rl32(pb);
                parse_media_type(s, 0, sid, mediatype, subtype, formattype, size);
                consumed += 92 + size;
            }
        } else if (!ff_guidcmp(g, stream2_guid)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (stream_index >= 0 && !((WtvStream*)s->streams[stream_index]->priv_data)->seen_data) {
                ff_asf_guid mediatype, subtype, formattype;
                int size;
                avio_skip(pb, 12);
                ff_get_guid(pb, &mediatype);
                ff_get_guid(pb, &subtype);
                avio_skip(pb, 12);
                ff_get_guid(pb, &formattype);
                size = avio_rl32(pb);
                parse_media_type(s, s->streams[stream_index], sid, mediatype, subtype, formattype, size);
                consumed += 76 + size;
            }
        } else if (!ff_guidcmp(g, EVENTID_AudioDescriptorSpanningEvent) ||
                   !ff_guidcmp(g, EVENTID_CtxADescriptorSpanningEvent) ||
                   !ff_guidcmp(g, EVENTID_CSDescriptorSpanningEvent) ||
                   !ff_guidcmp(g, EVENTID_StreamIDSpanningEvent) ||
                   !ff_guidcmp(g, EVENTID_SubtitleSpanningEvent) ||
                   !ff_guidcmp(g, EVENTID_TeletextSpanningEvent)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (stream_index >= 0) {
                AVStream *st = s->streams[stream_index];
                uint8_t buf[258];
                const uint8_t *pbuf = buf;
                int buf_size;

                avio_skip(pb, 8);
                consumed += 8;
                if (!ff_guidcmp(g, EVENTID_CtxADescriptorSpanningEvent) ||
                    !ff_guidcmp(g, EVENTID_CSDescriptorSpanningEvent)) {
                    avio_skip(pb, 6);
                    consumed += 6;
                }

                buf_size = FFMIN(len - consumed, sizeof(buf));
                avio_read(pb, buf, buf_size);
                consumed += buf_size;
                ff_parse_mpeg2_descriptor(s, st, 0, &pbuf, buf + buf_size, NULL, 0, 0, NULL);
            }
        } else if (!ff_guidcmp(g, EVENTID_AudioTypeSpanningEvent)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (stream_index >= 0) {
                AVStream *st = s->streams[stream_index];
                int audio_type;
                avio_skip(pb, 8);
                audio_type = avio_r8(pb);
                if (audio_type == 2)
                    st->disposition |= AV_DISPOSITION_HEARING_IMPAIRED;
                else if (audio_type == 3)
                    st->disposition |= AV_DISPOSITION_VISUAL_IMPAIRED;
                consumed += 9;
            }
        } else if (!ff_guidcmp(g, EVENTID_DVBScramblingControlSpanningEvent)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (stream_index >= 0) {
                avio_skip(pb, 12);
                if (avio_rl32(pb))
                    av_log(s, AV_LOG_WARNING, "DVB scrambled stream detected (st:%d), decoding will likely fail\n", stream_index);
                consumed += 16;
            }
        } else if (!ff_guidcmp(g, EVENTID_LanguageSpanningEvent)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (stream_index >= 0) {
                AVStream *st = s->streams[stream_index];
                uint8_t language[4];
                avio_skip(pb, 12);
                avio_read(pb, language, 3);
                if (language[0]) {
                    language[3] = 0;
                    av_dict_set(&st->metadata, "language", language, 0);
                    if (!strcmp(language, "nar") || !strcmp(language, "NAR"))
                        st->disposition |= AV_DISPOSITION_VISUAL_IMPAIRED;
                }
                consumed += 15;
            }
        } else if (!ff_guidcmp(g, timestamp_guid)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (stream_index >= 0) {
                avio_skip(pb, 8);
                wtv->pts = avio_rl64(pb);
                consumed += 16;
                if (wtv->pts == -1)
                    wtv->pts = AV_NOPTS_VALUE;
                else {
                    wtv->last_valid_pts = wtv->pts;
                    if (wtv->epoch == AV_NOPTS_VALUE || wtv->pts < wtv->epoch)
                        wtv->epoch = wtv->pts;
                if (mode == SEEK_TO_PTS && wtv->pts >= seekts) {
#define WTV_PAD8(x) (((x) + 7) & ~7)
                    avio_skip(pb, WTV_PAD8(len) - consumed);
                    return 0;
                }
                }
            }
        } else if (!ff_guidcmp(g, data_guid)) {
            int stream_index = ff_find_stream_index(s, sid);
            if (mode == SEEK_TO_DATA && stream_index >= 0 && len > 32) {
                WtvStream *wst = s->streams[stream_index]->priv_data;
                wst->seen_data = 1;
                if (len_ptr) {
                    *len_ptr = len;
                }
                return stream_index;
            }
        } else if (
            !ff_guidcmp(g, /* DSATTRIB_CAPTURE_STREAMTIME */ (const ff_asf_guid){0x14,0x56,0x1A,0x0C,0xCD,0x30,0x40,0x4F,0xBC,0xBF,0xD0,0x3E,0x52,0x30,0x62,0x07}) ||
            !ff_guidcmp(g, /* DSATTRIB_PicSampleSeq */ (const ff_asf_guid){0x02,0xAE,0x5B,0x2F,0x8F,0x7B,0x60,0x4F,0x82,0xD6,0xE4,0xEA,0x2F,0x1F,0x4C,0x99}) ||
            !ff_guidcmp(g, /* DSATTRIB_TRANSPORT_PROPERTIES */ (const ff_asf_guid){0x12,0xF6,0x22,0xB6,0xAD,0x47,0x71,0x46,0xAD,0x6C,0x05,0xA9,0x8E,0x65,0xDE,0x3A}) ||
            !ff_guidcmp(g, /* dvr_ms_vid_frame_rep_data */ (const ff_asf_guid){0xCC,0x32,0x64,0xDD,0x29,0xE2,0xDB,0x40,0x80,0xF6,0xD2,0x63,0x28,0xD2,0x76,0x1F}) ||
            !ff_guidcmp(g, /* EVENTID_ChannelChangeSpanningEvent */ (const ff_asf_guid){0xE5,0xC5,0x67,0x90,0x5C,0x4C,0x05,0x42,0x86,0xC8,0x7A,0xFE,0x20,0xFE,0x1E,0xFA}) ||
            !ff_guidcmp(g, /* EVENTID_ChannelInfoSpanningEvent */ (const ff_asf_guid){0x80,0x6D,0xF3,0x41,0x32,0x41,0xC2,0x4C,0xB1,0x21,0x01,0xA4,0x32,0x19,0xD8,0x1B}) ||
            !ff_guidcmp(g, /* EVENTID_ChannelTypeSpanningEvent */ (const ff_asf_guid){0x51,0x1D,0xAB,0x72,0xD2,0x87,0x9B,0x48,0xBA,0x11,0x0E,0x08,0xDC,0x21,0x02,0x43}) ||
            !ff_guidcmp(g, /* EVENTID_PIDListSpanningEvent */ (const ff_asf_guid){0x65,0x8F,0xFC,0x47,0xBB,0xE2,0x34,0x46,0x9C,0xEF,0xFD,0xBF,0xE6,0x26,0x1D,0x5C}) ||
            !ff_guidcmp(g, /* EVENTID_SignalAndServiceStatusSpanningEvent */ (const ff_asf_guid){0xCB,0xC5,0x68,0x80,0x04,0x3C,0x2B,0x49,0xB4,0x7D,0x03,0x08,0x82,0x0D,0xCE,0x51}) ||
            !ff_guidcmp(g, /* EVENTID_StreamTypeSpanningEvent */ (const ff_asf_guid){0xBC,0x2E,0xAF,0x82,0xA6,0x30,0x64,0x42,0xA8,0x0B,0xAD,0x2E,0x13,0x72,0xAC,0x60}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x1E,0xBE,0xC3,0xC5,0x43,0x92,0xDC,0x11,0x85,0xE5,0x00,0x12,0x3F,0x6F,0x73,0xB9}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x3B,0x86,0xA2,0xB1,0xEB,0x1E,0xC3,0x44,0x8C,0x88,0x1C,0xA3,0xFF,0xE3,0xE7,0x6A}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x4E,0x7F,0x4C,0x5B,0xC4,0xD0,0x38,0x4B,0xA8,0x3E,0x21,0x7F,0x7B,0xBF,0x52,0xE7}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x63,0x36,0xEB,0xFE,0xA1,0x7E,0xD9,0x11,0x83,0x08,0x00,0x07,0xE9,0x5E,0xAD,0x8D}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x70,0xE9,0xF1,0xF8,0x89,0xA4,0x4C,0x4D,0x83,0x73,0xB8,0x12,0xE0,0xD5,0xF8,0x1E}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x96,0xC3,0xD2,0xC2,0x7E,0x9A,0xDA,0x11,0x8B,0xF7,0x00,0x07,0xE9,0x5E,0xAD,0x8D}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0x97,0xC3,0xD2,0xC2,0x7E,0x9A,0xDA,0x11,0x8B,0xF7,0x00,0x07,0xE9,0x5E,0xAD,0x8D}) ||
            !ff_guidcmp(g, (const ff_asf_guid){0xA1,0xC3,0xD2,0xC2,0x7E,0x9A,0xDA,0x11,0x8B,0xF7,0x00,0x07,0xE9,0x5E,0xAD,0x8D})) {
            //ignore known guids
        } else
            av_log(s, AV_LOG_WARNING, "unsupported chunk:"FF_PRI_GUID"\n", FF_ARG_GUID(g));

        avio_skip(pb, WTV_PAD8(len) - consumed);
    }
    return AVERROR_EOF;
}

/* declare utf16le strings */
#define _ , 0,
static const uint8_t timeline_le16[] =
    {'t'_'i'_'m'_'e'_'l'_'i'_'n'_'e', 0};
static const uint8_t table_0_entries_legacy_attrib_le16[] =
    {'t'_'a'_'b'_'l'_'e'_'.'_'0'_'.'_'e'_'n'_'t'_'r'_'i'_'e'_'s'_'.'_'l'_'e'_'g'_'a'_'c'_'y'_'_'_'a'_'t'_'t'_'r'_'i'_'b', 0};
static const uint8_t table_0_entries_time_le16[] =
    {'t'_'a'_'b'_'l'_'e'_'.'_'0'_'.'_'e'_'n'_'t'_'r'_'i'_'e'_'s'_'.'_'t'_'i'_'m'_'e', 0};
static const uint8_t timeline_table_0_entries_Events_le16[] =
    {'t'_'i'_'m'_'e'_'l'_'i'_'n'_'e'_'.'_'t'_'a'_'b'_'l'_'e'_'.'_'0'_'.'_'e'_'n'_'t'_'r'_'i'_'e'_'s'_'.'_'E'_'v'_'e'_'n'_'t'_'s', 0};
#undef _

static int read_header(AVFormatContext *s)
{
    WtvContext *wtv = s->priv_data;
    int root_sector, root_size;
    uint8_t root[WTV_SECTOR_SIZE];
    AVIOContext *pb;
    int64_t timeline_pos;
    int ret;

    wtv->epoch          =
    wtv->pts            =
    wtv->last_valid_pts = AV_NOPTS_VALUE;

    /* read root directory sector */
    avio_skip(s->pb, 0x30);
    root_size = avio_rl32(s->pb);
    if (root_size > sizeof(root)) {
        av_log(s, AV_LOG_ERROR, "root directory size exceeds sector size\n");
        return AVERROR_INVALIDDATA;
    }
    avio_skip(s->pb, 4);
    root_sector = avio_rl32(s->pb);

    seek_by_sector(s->pb, root_sector, 0);
    root_size = avio_read(s->pb, root, root_size);
    if (root_size < 0)
        return AVERROR_INVALIDDATA;

    /* parse chunks up until first data chunk */
    wtv->pb = wtvfile_open(s, root, root_size, timeline_le16);
    if (!wtv->pb) {
        av_log(s, AV_LOG_ERROR, "timeline data missing\n");
        return AVERROR_INVALIDDATA;
    }

    ret = parse_chunks(s, SEEK_TO_DATA, 0, 0);
    if (ret < 0)
        return ret;
    avio_seek(wtv->pb, -32, SEEK_CUR);

    timeline_pos = avio_tell(s->pb); // save before opening another file

    /* read metadata */
    pb = wtvfile_open(s, root, root_size, table_0_entries_legacy_attrib_le16);
    if (pb) {
        parse_legacy_attrib(s, pb);
        wtvfile_close(pb);
    }

    /* read seek index */
    if (s->nb_streams) {
        AVStream *st = s->streams[0];
        pb = wtvfile_open(s, root, root_size, table_0_entries_time_le16);
        if (pb) {
            while(1) {
                uint64_t timestamp = avio_rl64(pb);
                uint64_t frame_nb  = avio_rl64(pb);
                if (pb->eof_reached)
                    break;
                ff_add_index_entry(&wtv->index_entries, &wtv->nb_index_entries, &wtv->index_entries_allocated_size,
                                   0, timestamp, frame_nb, 0, AVINDEX_KEYFRAME);
            }
            wtvfile_close(pb);

            if (wtv->nb_index_entries) {
                pb = wtvfile_open(s, root, root_size, timeline_table_0_entries_Events_le16);
                if (pb) {
                    int i;
                    while (1) {
                        uint64_t frame_nb = avio_rl64(pb);
                        uint64_t position = avio_rl64(pb);
                        if (pb->eof_reached)
                            break;
                        for (i = wtv->nb_index_entries - 1; i >= 0; i--) {
                            AVIndexEntry *e = wtv->index_entries + i;
                            if (frame_nb > e->size)
                                break;
                            if (position > e->pos)
                                e->pos = position;
                        }
                    }
                    wtvfile_close(pb);
                    st->duration = wtv->index_entries[wtv->nb_index_entries - 1].timestamp;
                }
            }
        }
    }

    avio_seek(s->pb, timeline_pos, SEEK_SET);
    return 0;
}

static int read_packet(AVFormatContext *s, AVPacket *pkt)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;
    int stream_index, len, ret;

    stream_index = parse_chunks(s, SEEK_TO_DATA, 0, &len);
    if (stream_index < 0)
        return stream_index;

    ret = av_get_packet(pb, pkt, len - 32);
    if (ret < 0)
        return ret;
    pkt->stream_index = stream_index;
    pkt->pts          = wtv->pts;
    avio_skip(pb, WTV_PAD8(len) - len);
    return 0;
}

static int read_seek(AVFormatContext *s, int stream_index,
                     int64_t ts, int flags)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;
    AVStream *st = s->streams[0];
    int64_t ts_relative;
    int i;

    if ((flags & AVSEEK_FLAG_FRAME) || (flags & AVSEEK_FLAG_BYTE))
        return AVERROR(ENOSYS);

    /* timestamp adjustment is required because wtv->pts values are absolute,
     * whereas AVIndexEntry->timestamp values are relative to epoch. */
    ts_relative = ts;
    if (wtv->epoch != AV_NOPTS_VALUE)
        ts_relative -= wtv->epoch;

    i = ff_index_search_timestamp(wtv->index_entries, wtv->nb_index_entries, ts_relative, flags);
    if (i < 0) {
        if (wtv->last_valid_pts == AV_NOPTS_VALUE || ts < wtv->last_valid_pts)
            avio_seek(pb, 0, SEEK_SET);
        else if (st->duration != AV_NOPTS_VALUE && ts_relative > st->duration && wtv->nb_index_entries)
            avio_seek(pb, wtv->index_entries[wtv->nb_index_entries - 1].pos, SEEK_SET);
        if (parse_chunks(s, SEEK_TO_PTS, ts, 0) < 0)
            return AVERROR(ERANGE);
        return 0;
    }
    wtv->pts = wtv->index_entries[i].timestamp;
    if (wtv->epoch != AV_NOPTS_VALUE)
        wtv->pts += wtv->epoch;
    wtv->last_valid_pts = wtv->pts;
    avio_seek(pb, wtv->index_entries[i].pos, SEEK_SET);
    return 0;
}

static int read_close(AVFormatContext *s)
{
    WtvContext *wtv = s->priv_data;
    av_free(wtv->index_entries);
    wtvfile_close(wtv->pb);
    return 0;
}

AVInputFormat ff_wtv_demuxer = {
    .name           = "wtv",
    .long_name      = NULL_IF_CONFIG_SMALL("Windows Television (WTV)"),
    .priv_data_size = sizeof(WtvContext),
    .read_probe     = read_probe,
    .read_header    = read_header,
    .read_packet    = read_packet,
    .read_seek      = read_seek,
    .read_close     = read_close,
    .flags          = AVFMT_SHOW_IDS,
};

//						↓↓↓VULNERABLE LINES↓↓↓

// 514,8;514,14

