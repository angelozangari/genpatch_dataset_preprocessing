/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * getchar.c
 *
 * functions related with getting a character from the user/mapping/redo/...
 *
 * manipulations with redo buffer and stuff buffer
 * mappings and abbreviations
 */

#include "vim.h"

/*
 * These buffers are used for storing:
 * - stuffed characters: A command that is translated into another command.
 * - redo characters: will redo the last change.
 * - recorded characters: for the "q" command.
 *
 * The bytes are stored like in the typeahead buffer:
 * - K_SPECIAL introduces a special key (two more bytes follow).  A literal
 *   K_SPECIAL is stored as K_SPECIAL KS_SPECIAL KE_FILLER.
 * - CSI introduces a GUI termcap code (also when gui.in_use is FALSE,
 *   otherwise switching the GUI on would make mappings invalid).
 *   A literal CSI is stored as CSI KS_EXTRA KE_CSI.
 * These translations are also done on multi-byte characters!
 *
 * Escaping CSI bytes is done by the system-specific input functions, called
 * by ui_inchar().
 * Escaping K_SPECIAL is done by inchar().
 * Un-escaping is done by vgetc().
 */

#define MINIMAL_SIZE 20			/* minimal size for b_str */

static buffheader_T redobuff = {{NULL, {NUL}}, NULL, 0, 0};
static buffheader_T old_redobuff = {{NULL, {NUL}}, NULL, 0, 0};
#if defined(FEAT_AUTOCMD) || defined(FEAT_EVAL) || defined(PROTO)
static buffheader_T save_redobuff = {{NULL, {NUL}}, NULL, 0, 0};
static buffheader_T save_old_redobuff = {{NULL, {NUL}}, NULL, 0, 0};
#endif
static buffheader_T recordbuff = {{NULL, {NUL}}, NULL, 0, 0};

static int typeahead_char = 0;		/* typeahead char that's not flushed */

/*
 * when block_redo is TRUE redo buffer will not be changed
 * used by edit() to repeat insertions and 'V' command for redoing
 */
static int	block_redo = FALSE;

/*
 * Make a hash value for a mapping.
 * "mode" is the lower 4 bits of the State for the mapping.
 * "c1" is the first character of the "lhs".
 * Returns a value between 0 and 255, index in maphash.
 * Put Normal/Visual mode mappings mostly separately from Insert/Cmdline mode.
 */
#define MAP_HASH(mode, c1) (((mode) & (NORMAL + VISUAL + SELECTMODE + OP_PENDING)) ? (c1) : ((c1) ^ 0x80))

/*
 * Each mapping is put in one of the 256 hash lists, to speed up finding it.
 */
static mapblock_T	*(maphash[256]);
static int		maphash_valid = FALSE;

/*
 * List used for abbreviations.
 */
static mapblock_T	*first_abbr = NULL; /* first entry in abbrlist */

static int		KeyNoremap = 0;	    /* remapping flags */

/*
 * variables used by vgetorpeek() and flush_buffers()
 *
 * typebuf.tb_buf[] contains all characters that are not consumed yet.
 * typebuf.tb_buf[typebuf.tb_off] is the first valid character.
 * typebuf.tb_buf[typebuf.tb_off + typebuf.tb_len - 1] is the last valid char.
 * typebuf.tb_buf[typebuf.tb_off + typebuf.tb_len] must be NUL.
 * The head of the buffer may contain the result of mappings, abbreviations
 * and @a commands.  The length of this part is typebuf.tb_maplen.
 * typebuf.tb_silent is the part where <silent> applies.
 * After the head are characters that come from the terminal.
 * typebuf.tb_no_abbr_cnt is the number of characters in typebuf.tb_buf that
 * should not be considered for abbreviations.
 * Some parts of typebuf.tb_buf may not be mapped. These parts are remembered
 * in typebuf.tb_noremap[], which is the same length as typebuf.tb_buf and
 * contains RM_NONE for the characters that are not to be remapped.
 * typebuf.tb_noremap[typebuf.tb_off] is the first valid flag.
 * (typebuf has been put in globals.h, because check_termcode() needs it).
 */
#define RM_YES		0	/* tb_noremap: remap */
#define RM_NONE		1	/* tb_noremap: don't remap */
#define RM_SCRIPT	2	/* tb_noremap: remap local script mappings */
#define RM_ABBR		4	/* tb_noremap: don't remap, do abbrev. */

/* typebuf.tb_buf has three parts: room in front (for result of mappings), the
 * middle for typeahead and room for new characters (which needs to be 3 *
 * MAXMAPLEN) for the Amiga).
 */
#define TYPELEN_INIT	(5 * (MAXMAPLEN + 3))
static char_u	typebuf_init[TYPELEN_INIT];	/* initial typebuf.tb_buf */
static char_u	noremapbuf_init[TYPELEN_INIT];	/* initial typebuf.tb_noremap */

static int	last_recorded_len = 0;	/* number of last recorded chars */

static char_u	*get_buffcont(buffheader_T *, int);
static void	add_buff(buffheader_T *, char_u *, long n);
static void	add_num_buff(buffheader_T *, long);
static void	add_char_buff(buffheader_T *, int);
static int	read_readbuffers(int advance);
static int	read_readbuf(buffheader_T *buf, int advance);
static void	start_stuff(void);
static int	read_redo(int, int);
static void	copy_redo(int);
static void	init_typebuf(void);
static void	gotchars(char_u *, int);
static void	may_sync_undo(void);
static void	closescript(void);
static int	vgetorpeek(int);
static void	map_free(mapblock_T **);
static void	validate_maphash(void);
static void	showmap(mapblock_T *mp, int local);
#ifdef FEAT_EVAL
static char_u	*eval_map_expr(char_u *str, int c);
#endif

/*
 * Free and clear a buffer.
 */
    void
free_buff(buffheader_T *buf)
{
    buffblock_T	*p, *np;

    for (p = buf->bh_first.b_next; p != NULL; p = np)
    {
	np = p->b_next;
	vim_free(p);
    }
    buf->bh_first.b_next = NULL;
}

/*
 * Return the contents of a buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    static char_u *
get_buffcont(
    buffheader_T	*buffer,
    int			dozero)	    /* count == zero is not an error */
{
    long_u	    count = 0;
    char_u	    *p = NULL;
    char_u	    *p2;
    char_u	    *str;
    buffblock_T *bp;

    /* compute the total length of the string */
    for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
	count += (long_u)STRLEN(bp->b_str);

    if ((count || dozero) && (p = lalloc(count + 1, TRUE)) != NULL)
    {
	p2 = p;
	for (bp = buffer->bh_first.b_next; bp != NULL; bp = bp->b_next)
	    for (str = bp->b_str; *str; )
		*p2++ = *str++;
	*p2 = NUL;
    }
    return (p);
}

/*
 * Return the contents of the record buffer as a single string
 * and clear the record buffer.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_recorded(void)
{
    char_u	*p;
    size_t	len;

    p = get_buffcont(&recordbuff, TRUE);
    free_buff(&recordbuff);

    /*
     * Remove the characters that were added the last time, these must be the
     * (possibly mapped) characters that stopped the recording.
     */
    len = STRLEN(p);
    if ((int)len >= last_recorded_len)
    {
	len -= last_recorded_len;
	p[len] = NUL;
    }

    /*
     * When stopping recording from Insert mode with CTRL-O q, also remove the
     * CTRL-O.
     */
    if (len > 0 && restart_edit != 0 && p[len - 1] == Ctrl_O)
	p[len - 1] = NUL;

    return (p);
}

/*
 * Return the contents of the redo buffer as a single string.
 * K_SPECIAL and CSI in the returned string are escaped.
 */
    char_u *
get_inserted(void)
{
    return get_buffcont(&redobuff, FALSE);
}

/*
 * Add string "s" after the current block of buffer "buf".
 * K_SPECIAL and CSI should have been escaped already.
 */
    static void
add_buff(
    buffheader_T	*buf,
    char_u		*s,
    long		slen)	/* length of "s" or -1 */
{
    buffblock_T *p;
    long_u	    len;

    if (slen < 0)
	slen = (long)STRLEN(s);
    if (slen == 0)				/* don't add empty strings */
	return;

    if (buf->bh_first.b_next == NULL)	/* first add to list */
    {
	buf->bh_space = 0;
	buf->bh_curr = &(buf->bh_first);
    }
    else if (buf->bh_curr == NULL)	/* buffer has already been read */
    {
	EMSG(_("E222: Add to read buffer"));
	return;
    }
    else if (buf->bh_index != 0)
	mch_memmove(buf->bh_first.b_next->b_str,
		    buf->bh_first.b_next->b_str + buf->bh_index,
		    STRLEN(buf->bh_first.b_next->b_str + buf->bh_index) + 1);
    buf->bh_index = 0;

    if (buf->bh_space >= (int)slen)
    {
	len = (long_u)STRLEN(buf->bh_curr->b_str);
	vim_strncpy(buf->bh_curr->b_str + len, s, (size_t)slen);
	buf->bh_space -= slen;
    }
    else
    {
	if (slen < MINIMAL_SIZE)
	    len = MINIMAL_SIZE;
	else
	    len = slen;
	p = (buffblock_T *)lalloc((long_u)(sizeof(buffblock_T) + len),
									TRUE);
	if (p == NULL)
	    return; /* no space, just forget it */
	buf->bh_space = (int)(len - slen);
	vim_strncpy(p->b_str, s, (size_t)slen);

	p->b_next = buf->bh_curr->b_next;
	buf->bh_curr->b_next = p;
	buf->bh_curr = p;
    }
    return;
}

/*
 * Add number "n" to buffer "buf".
 */
    static void
add_num_buff(buffheader_T *buf, long n)
{
    char_u	number[32];

    sprintf((char *)number, "%ld", n);
    add_buff(buf, number, -1L);
}

/*
 * Add character 'c' to buffer "buf".
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    static void
add_char_buff(buffheader_T *buf, int c)
{
#ifdef FEAT_MBYTE
    char_u	bytes[MB_MAXBYTES + 1];
    int		len;
    int		i;
#endif
    char_u	temp[4];

#ifdef FEAT_MBYTE
    if (IS_SPECIAL(c))
	len = 1;
    else
	len = (*mb_char2bytes)(c, bytes);
    for (i = 0; i < len; ++i)
    {
	if (!IS_SPECIAL(c))
	    c = bytes[i];
#endif

	if (IS_SPECIAL(c) || c == K_SPECIAL || c == NUL)
	{
	    /* translate special key code into three byte sequence */
	    temp[0] = K_SPECIAL;
	    temp[1] = K_SECOND(c);
	    temp[2] = K_THIRD(c);
	    temp[3] = NUL;
	}
#ifdef FEAT_GUI
	else if (c == CSI)
	{
	    /* Translate a CSI to a CSI - KS_EXTRA - KE_CSI sequence */
	    temp[0] = CSI;
	    temp[1] = KS_EXTRA;
	    temp[2] = (int)KE_CSI;
	    temp[3] = NUL;
	}
#endif
	else
	{
	    temp[0] = c;
	    temp[1] = NUL;
	}
	add_buff(buf, temp, -1L);
#ifdef FEAT_MBYTE
    }
#endif
}

/* First read ahead buffer. Used for translated commands. */
static buffheader_T readbuf1 = {{NULL, {NUL}}, NULL, 0, 0};

/* Second read ahead buffer. Used for redo. */
static buffheader_T readbuf2 = {{NULL, {NUL}}, NULL, 0, 0};

/*
 * Get one byte from the read buffers.  Use readbuf1 one first, use readbuf2
 * if that one is empty.
 * If advance == TRUE go to the next char.
 * No translation is done K_SPECIAL and CSI are escaped.
 */
    static int
read_readbuffers(int advance)
{
    int c;

    c = read_readbuf(&readbuf1, advance);
    if (c == NUL)
	c = read_readbuf(&readbuf2, advance);
    return c;
}

    static int
read_readbuf(buffheader_T *buf, int advance)
{
    char_u	c;
    buffblock_T	*curr;

    if (buf->bh_first.b_next == NULL)  /* buffer is empty */
	return NUL;

    curr = buf->bh_first.b_next;
    c = curr->b_str[buf->bh_index];

    if (advance)
    {
	if (curr->b_str[++buf->bh_index] == NUL)
	{
	    buf->bh_first.b_next = curr->b_next;
	    vim_free(curr);
	    buf->bh_index = 0;
	}
    }
    return c;
}

/*
 * Prepare the read buffers for reading (if they contain something).
 */
    static void
start_stuff(void)
{
    if (readbuf1.bh_first.b_next != NULL)
    {
	readbuf1.bh_curr = &(readbuf1.bh_first);
	readbuf1.bh_space = 0;
    }
    if (readbuf2.bh_first.b_next != NULL)
    {
	readbuf2.bh_curr = &(readbuf2.bh_first);
	readbuf2.bh_space = 0;
    }
}

/*
 * Return TRUE if the stuff buffer is empty.
 */
    int
stuff_empty(void)
{
    return (readbuf1.bh_first.b_next == NULL
	 && readbuf2.bh_first.b_next == NULL);
}

/*
 * Return TRUE if readbuf1 is empty.  There may still be redo characters in
 * redbuf2.
 */
    int
readbuf1_empty(void)
{
    return (readbuf1.bh_first.b_next == NULL);
}

/*
 * Set a typeahead character that won't be flushed.
 */
    void
typeahead_noflush(int c)
{
    typeahead_char = c;
}

/*
 * Remove the contents of the stuff buffer and the mapped characters in the
 * typeahead buffer (used in case of an error).  If "flush_typeahead" is true,
 * flush all typeahead characters (used when interrupted by a CTRL-C).
 */
    void
flush_buffers(int flush_typeahead)
{
    init_typebuf();

    start_stuff();
    while (read_readbuffers(TRUE) != NUL)
	;

    if (flush_typeahead)	    /* remove all typeahead */
    {
	/*
	 * We have to get all characters, because we may delete the first part
	 * of an escape sequence.
	 * In an xterm we get one char at a time and we have to get them all.
	 */
	while (inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 10L,
						  typebuf.tb_change_cnt) != 0)
	    ;
	typebuf.tb_off = MAXMAPLEN;
	typebuf.tb_len = 0;
    }
    else		    /* remove mapped characters at the start only */
    {
	typebuf.tb_off += typebuf.tb_maplen;
	typebuf.tb_len -= typebuf.tb_maplen;
    }
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    cmd_silent = FALSE;
    typebuf.tb_no_abbr_cnt = 0;
}

/*
 * The previous contents of the redo buffer is kept in old_redobuffer.
 * This is used for the CTRL-O <.> command in insert mode.
 */
    void
ResetRedobuff(void)
{
    if (!block_redo)
    {
	free_buff(&old_redobuff);
	old_redobuff = redobuff;
	redobuff.bh_first.b_next = NULL;
    }
}

/*
 * Discard the contents of the redo buffer and restore the previous redo
 * buffer.
 */
    void
CancelRedo(void)
{
    if (!block_redo)
    {
	free_buff(&redobuff);
	redobuff = old_redobuff;
	old_redobuff.bh_first.b_next = NULL;
	start_stuff();
	while (read_readbuffers(TRUE) != NUL)
	    ;
    }
}

#if defined(FEAT_AUTOCMD) || defined(FEAT_EVAL) || defined(PROTO)
/*
 * Save redobuff and old_redobuff to save_redobuff and save_old_redobuff.
 * Used before executing autocommands and user functions.
 */
static int save_level = 0;

    void
saveRedobuff(void)
{
    char_u	*s;

    if (save_level++ == 0)
    {
	save_redobuff = redobuff;
	redobuff.bh_first.b_next = NULL;
	save_old_redobuff = old_redobuff;
	old_redobuff.bh_first.b_next = NULL;

	/* Make a copy, so that ":normal ." in a function works. */
	s = get_buffcont(&save_redobuff, FALSE);
	if (s != NULL)
	{
	    add_buff(&redobuff, s, -1L);
	    vim_free(s);
	}
    }
}

/*
 * Restore redobuff and old_redobuff from save_redobuff and save_old_redobuff.
 * Used after executing autocommands and user functions.
 */
    void
restoreRedobuff(void)
{
    if (--save_level == 0)
    {
	free_buff(&redobuff);
	redobuff = save_redobuff;
	free_buff(&old_redobuff);
	old_redobuff = save_old_redobuff;
    }
}
#endif

/*
 * Append "s" to the redo buffer.
 * K_SPECIAL and CSI should already have been escaped.
 */
    void
AppendToRedobuff(char_u *s)
{
    if (!block_redo)
	add_buff(&redobuff, s, -1L);
}

/*
 * Append to Redo buffer literally, escaping special characters with CTRL-V.
 * K_SPECIAL and CSI are escaped as well.
 */
    void
AppendToRedobuffLit(
    char_u	*str,
    int		len)	    /* length of "str" or -1 for up to the NUL */
{
    char_u	*s = str;
    int		c;
    char_u	*start;

    if (block_redo)
	return;

    while (len < 0 ? *s != NUL : s - str < len)
    {
	/* Put a string of normal characters in the redo buffer (that's
	 * faster). */
	start = s;
	while (*s >= ' '
#ifndef EBCDIC
		&& *s < DEL	/* EBCDIC: all chars above space are normal */
#endif
		&& (len < 0 || s - str < len))
	    ++s;

	/* Don't put '0' or '^' as last character, just in case a CTRL-D is
	 * typed next. */
	if (*s == NUL && (s[-1] == '0' || s[-1] == '^'))
	    --s;
	if (s > start)
	    add_buff(&redobuff, start, (long)(s - start));

	if (*s == NUL || (len >= 0 && s - str >= len))
	    break;

	/* Handle a special or multibyte character. */
#ifdef FEAT_MBYTE
	if (has_mbyte)
	    /* Handle composing chars separately. */
	    c = mb_cptr2char_adv(&s);
	else
#endif
	    c = *s++;
	if (c < ' ' || c == DEL || (*s == NUL && (c == '0' || c == '^')))
	    add_char_buff(&redobuff, Ctrl_V);

	/* CTRL-V '0' must be inserted as CTRL-V 048 (EBCDIC: xf0) */
	if (*s == NUL && c == '0')
#ifdef EBCDIC
	    add_buff(&redobuff, (char_u *)"xf0", 3L);
#else
	    add_buff(&redobuff, (char_u *)"048", 3L);
#endif
	else
	    add_char_buff(&redobuff, c);
    }
}

/*
 * Append a character to the redo buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    void
AppendCharToRedobuff(int c)
{
    if (!block_redo)
	add_char_buff(&redobuff, c);
}

/*
 * Append a number to the redo buffer.
 */
    void
AppendNumberToRedobuff(long n)
{
    if (!block_redo)
	add_num_buff(&redobuff, n);
}

/*
 * Append string "s" to the stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    void
stuffReadbuff(char_u *s)
{
    add_buff(&readbuf1, s, -1L);
}

/*
 * Append string "s" to the redo stuff buffer.
 * CSI and K_SPECIAL must already have been escaped.
 */
    void
stuffRedoReadbuff(char_u *s)
{
    add_buff(&readbuf2, s, -1L);
}

    void
stuffReadbuffLen(char_u *s, long len)
{
    add_buff(&readbuf1, s, len);
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Stuff "s" into the stuff buffer, leaving special key codes unmodified and
 * escaping other K_SPECIAL and CSI bytes.
 * Change CR, LF and ESC into a space.
 */
    void
stuffReadbuffSpec(char_u *s)
{
    int c;

    while (*s != NUL)
    {
	if (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    /* Insert special key literally. */
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	else
	{
#ifdef FEAT_MBYTE
	    c = mb_ptr2char_adv(&s);
#else
	    c = *s++;
#endif
	    if (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}
#endif

/*
 * Append a character to the stuff buffer.
 * Translates special keys, NUL, CSI, K_SPECIAL and multibyte characters.
 */
    void
stuffcharReadbuff(int c)
{
    add_char_buff(&readbuf1, c);
}

/*
 * Append a number to the stuff buffer.
 */
    void
stuffnumReadbuff(long n)
{
    add_num_buff(&readbuf1, n);
}

/*
 * Read a character from the redo buffer.  Translates K_SPECIAL, CSI and
 * multibyte characters.
 * The redo buffer is left as it is.
 * If init is TRUE, prepare for redo, return FAIL if nothing to redo, OK
 * otherwise.
 * If old is TRUE, use old_redobuff instead of redobuff.
 */
    static int
read_redo(int init, int old_redo)
{
    static buffblock_T	*bp;
    static char_u	*p;
    int			c;
#ifdef FEAT_MBYTE
    int			n;
    char_u		buf[MB_MAXBYTES + 1];
    int			i;
#endif

    if (init)
    {
	if (old_redo)
	    bp = old_redobuff.bh_first.b_next;
	else
	    bp = redobuff.bh_first.b_next;
	if (bp == NULL)
	    return FAIL;
	p = bp->b_str;
	return OK;
    }
    if ((c = *p) != NUL)
    {
	/* Reverse the conversion done by add_char_buff() */
#ifdef FEAT_MBYTE
	/* For a multi-byte character get all the bytes and return the
	 * converted character. */
	if (has_mbyte && (c != K_SPECIAL || p[1] == KS_SPECIAL))
	    n = MB_BYTE2LEN_CHECK(c);
	else
	    n = 1;
	for (i = 0; ; ++i)
#endif
	{
	    if (c == K_SPECIAL) /* special key or escaped K_SPECIAL */
	    {
		c = TO_SPECIAL(p[1], p[2]);
		p += 2;
	    }
#ifdef FEAT_GUI
	    if (c == CSI)	/* escaped CSI */
		p += 2;
#endif
	    if (*++p == NUL && bp->b_next != NULL)
	    {
		bp = bp->b_next;
		p = bp->b_str;
	    }
#ifdef FEAT_MBYTE
	    buf[i] = c;
	    if (i == n - 1)	/* last byte of a character */
	    {
		if (n != 1)
		    c = (*mb_ptr2char)(buf);
		break;
	    }
	    c = *p;
	    if (c == NUL)	/* cannot happen? */
		break;
#endif
	}
    }

    return c;
}

/*
 * Copy the rest of the redo buffer into the stuff buffer (in a slow way).
 * If old_redo is TRUE, use old_redobuff instead of redobuff.
 * The escaped K_SPECIAL and CSI are copied without translation.
 */
    static void
copy_redo(int old_redo)
{
    int	    c;

    while ((c = read_redo(FALSE, old_redo)) != NUL)
	add_char_buff(&readbuf2, c);
}

/*
 * Stuff the redo buffer into readbuf2.
 * Insert the redo count into the command.
 * If "old_redo" is TRUE, the last but one command is repeated
 * instead of the last command (inserting text). This is used for
 * CTRL-O <.> in insert mode
 *
 * return FAIL for failure, OK otherwise
 */
    int
start_redo(long count, int old_redo)
{
    int	    c;

    /* init the pointers; return if nothing to redo */
    if (read_redo(TRUE, old_redo) == FAIL)
	return FAIL;

    c = read_redo(FALSE, old_redo);

    /* copy the buffer name, if present */
    if (c == '"')
    {
	add_buff(&readbuf2, (char_u *)"\"", 1L);
	c = read_redo(FALSE, old_redo);

	/* if a numbered buffer is used, increment the number */
	if (c >= '1' && c < '9')
	    ++c;
	add_char_buff(&readbuf2, c);
	c = read_redo(FALSE, old_redo);
    }

    if (c == 'v')   /* redo Visual */
    {
	VIsual = curwin->w_cursor;
	VIsual_active = TRUE;
	VIsual_select = FALSE;
	VIsual_reselect = TRUE;
	redo_VIsual_busy = TRUE;
	c = read_redo(FALSE, old_redo);
    }

    /* try to enter the count (in place of a previous count) */
    if (count)
    {
	while (VIM_ISDIGIT(c))	/* skip "old" count */
	    c = read_redo(FALSE, old_redo);
	add_num_buff(&readbuf2, count);
    }

    /* copy from the redo buffer into the stuff buffer */
    add_char_buff(&readbuf2, c);
    copy_redo(old_redo);
    return OK;
}

/*
 * Repeat the last insert (R, o, O, a, A, i or I command) by stuffing
 * the redo buffer into readbuf2.
 * return FAIL for failure, OK otherwise
 */
    int
start_redo_ins(void)
{
    int	    c;

    if (read_redo(TRUE, FALSE) == FAIL)
	return FAIL;
    start_stuff();

    /* skip the count and the command character */
    while ((c = read_redo(FALSE, FALSE)) != NUL)
    {
	if (vim_strchr((char_u *)"AaIiRrOo", c) != NULL)
	{
	    if (c == 'O' || c == 'o')
		add_buff(&readbuf2, NL_STR, -1L);
	    break;
	}
    }

    /* copy the typed text from the redo buffer into the stuff buffer */
    copy_redo(FALSE);
    block_redo = TRUE;
    return OK;
}

    void
stop_redo_ins(void)
{
    block_redo = FALSE;
}

/*
 * Initialize typebuf.tb_buf to point to typebuf_init.
 * alloc() cannot be used here: In out-of-memory situations it would
 * be impossible to type anything.
 */
    static void
init_typebuf(void)
{
    if (typebuf.tb_buf == NULL)
    {
	typebuf.tb_buf = typebuf_init;
	typebuf.tb_noremap = noremapbuf_init;
	typebuf.tb_buflen = TYPELEN_INIT;
	typebuf.tb_len = 0;
	typebuf.tb_off = 0;
	typebuf.tb_change_cnt = 1;
    }
}

/*
 * Insert a string in position 'offset' in the typeahead buffer (for "@r"
 * and ":normal" command, vgetorpeek() and check_termcode()).
 *
 * If noremap is REMAP_YES, new string can be mapped again.
 * If noremap is REMAP_NONE, new string cannot be mapped again.
 * If noremap is REMAP_SKIP, fist char of new string cannot be mapped again,
 * but abbreviations are allowed.
 * If noremap is REMAP_SCRIPT, new string cannot be mapped again, except for
 *			script-local mappings.
 * If noremap is > 0, that many characters of the new string cannot be mapped.
 *
 * If nottyped is TRUE, the string does not return KeyTyped (don't use when
 * offset is non-zero!).
 *
 * If silent is TRUE, cmd_silent is set when the characters are obtained.
 *
 * return FAIL for failure, OK otherwise
 */
    int
ins_typebuf(
    char_u	*str,
    int		noremap,
    int		offset,
    int		nottyped,
    int		silent)
{
    char_u	*s1, *s2;
    int		newlen;
    int		addlen;
    int		i;
    int		newoff;
    int		val;
    int		nrm;

    init_typebuf();
    if (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;

    addlen = (int)STRLEN(str);

    /*
     * Easy case: there is room in front of typebuf.tb_buf[typebuf.tb_off]
     */
    if (offset == 0 && addlen <= typebuf.tb_off)
    {
	typebuf.tb_off -= addlen;
	mch_memmove(typebuf.tb_buf + typebuf.tb_off, str, (size_t)addlen);
    }

    /*
     * Need to allocate a new buffer.
     * In typebuf.tb_buf there must always be room for 3 * MAXMAPLEN + 4
     * characters.  We add some extra room to avoid having to allocate too
     * often.
     */
    else
    {
	newoff = MAXMAPLEN + 4;
	newlen = typebuf.tb_len + addlen + newoff + 4 * (MAXMAPLEN + 4);
	if (newlen < 0)		    /* string is getting too long */
	{
	    EMSG(_(e_toocompl));    /* also calls flush_buffers */
	    setcursor();
	    return FAIL;
	}
	s1 = alloc(newlen);
	if (s1 == NULL)		    /* out of memory */
	    return FAIL;
	s2 = alloc(newlen);
	if (s2 == NULL)		    /* out of memory */
	{
	    vim_free(s1);
	    return FAIL;
	}
	typebuf.tb_buflen = newlen;

	/* copy the old chars, before the insertion point */
	mch_memmove(s1 + newoff, typebuf.tb_buf + typebuf.tb_off,
							      (size_t)offset);
	/* copy the new chars */
	mch_memmove(s1 + newoff + offset, str, (size_t)addlen);
	/* copy the old chars, after the insertion point, including the	NUL at
	 * the end */
	mch_memmove(s1 + newoff + offset + addlen,
				     typebuf.tb_buf + typebuf.tb_off + offset,
				       (size_t)(typebuf.tb_len - offset + 1));
	if (typebuf.tb_buf != typebuf_init)
	    vim_free(typebuf.tb_buf);
	typebuf.tb_buf = s1;

	mch_memmove(s2 + newoff, typebuf.tb_noremap + typebuf.tb_off,
							      (size_t)offset);
	mch_memmove(s2 + newoff + offset + addlen,
		   typebuf.tb_noremap + typebuf.tb_off + offset,
					   (size_t)(typebuf.tb_len - offset));
	if (typebuf.tb_noremap != noremapbuf_init)
	    vim_free(typebuf.tb_noremap);
	typebuf.tb_noremap = s2;

	typebuf.tb_off = newoff;
    }
    typebuf.tb_len += addlen;

    /* If noremap == REMAP_SCRIPT: do remap script-local mappings. */
    if (noremap == REMAP_SCRIPT)
	val = RM_SCRIPT;
    else if (noremap == REMAP_SKIP)
	val = RM_ABBR;
    else
	val = RM_NONE;

    /*
     * Adjust typebuf.tb_noremap[] for the new characters:
     * If noremap == REMAP_NONE or REMAP_SCRIPT: new characters are
     *			(sometimes) not remappable
     * If noremap == REMAP_YES: all the new characters are mappable
     * If noremap  > 0: "noremap" characters are not remappable, the rest
     *			mappable
     */
    if (noremap == REMAP_SKIP)
	nrm = 1;
    else if (noremap < 0)
	nrm = addlen;
    else
	nrm = noremap;
    for (i = 0; i < addlen; ++i)
	typebuf.tb_noremap[typebuf.tb_off + i + offset] =
						  (--nrm >= 0) ? val : RM_YES;

    /* tb_maplen and tb_silent only remember the length of mapped and/or
     * silent mappings at the start of the buffer, assuming that a mapped
     * sequence doesn't result in typed characters. */
    if (nottyped || typebuf.tb_maplen > offset)
	typebuf.tb_maplen += addlen;
    if (silent || typebuf.tb_silent > offset)
    {
	typebuf.tb_silent += addlen;
	cmd_silent = TRUE;
    }
    if (typebuf.tb_no_abbr_cnt && offset == 0)	/* and not used for abbrev.s */
	typebuf.tb_no_abbr_cnt += addlen;

    return OK;
}

/*
 * Put character "c" back into the typeahead buffer.
 * Can be used for a character obtained by vgetc() that needs to be put back.
 * Uses cmd_silent, KeyTyped and KeyNoremap to restore the flags belonging to
 * the char.
 */
    void
ins_char_typebuf(int c)
{
#ifdef FEAT_MBYTE
    char_u	buf[MB_MAXBYTES + 1];
#else
    char_u	buf[4];
#endif
    if (IS_SPECIAL(c))
    {
	buf[0] = K_SPECIAL;
	buf[1] = K_SECOND(c);
	buf[2] = K_THIRD(c);
	buf[3] = NUL;
    }
    else
    {
#ifdef FEAT_MBYTE
	buf[(*mb_char2bytes)(c, buf)] = NUL;
#else
	buf[0] = c;
	buf[1] = NUL;
#endif
    }
    (void)ins_typebuf(buf, KeyNoremap, 0, !KeyTyped, cmd_silent);
}

/*
 * Return TRUE if the typeahead buffer was changed (while waiting for a
 * character to arrive).  Happens when a message was received from a client or
 * from feedkeys().
 * But check in a more generic way to avoid trouble: When "typebuf.tb_buf"
 * changed it was reallocated and the old pointer can no longer be used.
 * Or "typebuf.tb_off" may have been changed and we would overwrite characters
 * that was just added.
 */
    int
typebuf_changed(
    int		tb_change_cnt)	/* old value of typebuf.tb_change_cnt */
{
    return (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#endif
	   ));
}

/*
 * Return TRUE if there are no characters in the typeahead buffer that have
 * not been typed (result from a mapping or come from ":normal").
 */
    int
typebuf_typed(void)
{
    return typebuf.tb_maplen == 0;
}

/*
 * Return the number of characters that are mapped (or not typed).
 */
    int
typebuf_maplen(void)
{
    return typebuf.tb_maplen;
}

/*
 * remove "len" characters from typebuf.tb_buf[typebuf.tb_off + offset]
 */
    void
del_typebuf(int len, int offset)
{
    int	    i;

    if (len == 0)
	return;		/* nothing to do */

    typebuf.tb_len -= len;

    /*
     * Easy case: Just increase typebuf.tb_off.
     */
    if (offset == 0 && typebuf.tb_buflen - (typebuf.tb_off + len)
							 >= 3 * MAXMAPLEN + 3)
	typebuf.tb_off += len;
    /*
     * Have to move the characters in typebuf.tb_buf[] and typebuf.tb_noremap[]
     */
    else
    {
	i = typebuf.tb_off + offset;
	/*
	 * Leave some extra room at the end to avoid reallocation.
	 */
	if (typebuf.tb_off > MAXMAPLEN)
	{
	    mch_memmove(typebuf.tb_buf + MAXMAPLEN,
			     typebuf.tb_buf + typebuf.tb_off, (size_t)offset);
	    mch_memmove(typebuf.tb_noremap + MAXMAPLEN,
			 typebuf.tb_noremap + typebuf.tb_off, (size_t)offset);
	    typebuf.tb_off = MAXMAPLEN;
	}
	/* adjust typebuf.tb_buf (include the NUL at the end) */
	mch_memmove(typebuf.tb_buf + typebuf.tb_off + offset,
						     typebuf.tb_buf + i + len,
				       (size_t)(typebuf.tb_len - offset + 1));
	/* adjust typebuf.tb_noremap[] */
	mch_memmove(typebuf.tb_noremap + typebuf.tb_off + offset,
						 typebuf.tb_noremap + i + len,
					   (size_t)(typebuf.tb_len - offset));
    }

    if (typebuf.tb_maplen > offset)		/* adjust tb_maplen */
    {
	if (typebuf.tb_maplen < offset + len)
	    typebuf.tb_maplen = offset;
	else
	    typebuf.tb_maplen -= len;
    }
    if (typebuf.tb_silent > offset)		/* adjust tb_silent */
    {
	if (typebuf.tb_silent < offset + len)
	    typebuf.tb_silent = offset;
	else
	    typebuf.tb_silent -= len;
    }
    if (typebuf.tb_no_abbr_cnt > offset)	/* adjust tb_no_abbr_cnt */
    {
	if (typebuf.tb_no_abbr_cnt < offset + len)
	    typebuf.tb_no_abbr_cnt = offset;
	else
	    typebuf.tb_no_abbr_cnt -= len;
    }

#if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
    /* Reset the flag that text received from a client or from feedkeys()
     * was inserted in the typeahead buffer. */
    typebuf_was_filled = FALSE;
#endif
    if (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
}

/*
 * Write typed characters to script file.
 * If recording is on put the character in the recordbuffer.
 */
    static void
gotchars(char_u *chars, int len)
{
    char_u	*s = chars;
    int		c;
    char_u	buf[2];
    int		todo = len;

    /* remember how many chars were last recorded */
    if (Recording)
	last_recorded_len += len;

    buf[1] = NUL;
    while (todo--)
    {
	/* Handle one byte at a time; no translation to be done. */
	c = *s++;
	updatescript(c);

	if (Recording)
	{
	    buf[0] = c;
	    add_buff(&recordbuff, buf, 1L);
	}
    }
    may_sync_undo();

#ifdef FEAT_EVAL
    /* output "debug mode" message next time in debug mode */
    debug_did_msg = FALSE;
#endif

    /* Since characters have been typed, consider the following to be in
     * another mapping.  Search string will be kept in history. */
    ++maptick;
}

/*
 * Sync undo.  Called when typed characters are obtained from the typeahead
 * buffer, or when a menu is used.
 * Do not sync:
 * - In Insert mode, unless cursor key has been used.
 * - While reading a script file.
 * - When no_u_sync is non-zero.
 */
    static void
may_sync_undo(void)
{
    if ((!(State & (INSERT + CMDLINE)) || arrow_used)
					       && scriptin[curscript] == NULL)
	u_sync(FALSE);
}

/*
 * Make "typebuf" empty and allocate new buffers.
 * Returns FAIL when out of memory.
 */
    int
alloc_typebuf(void)
{
    typebuf.tb_buf = alloc(TYPELEN_INIT);
    typebuf.tb_noremap = alloc(TYPELEN_INIT);
    if (typebuf.tb_buf == NULL || typebuf.tb_noremap == NULL)
    {
	free_typebuf();
	return FAIL;
    }
    typebuf.tb_buflen = TYPELEN_INIT;
    typebuf.tb_off = 0;
    typebuf.tb_len = 0;
    typebuf.tb_maplen = 0;
    typebuf.tb_silent = 0;
    typebuf.tb_no_abbr_cnt = 0;
    if (++typebuf.tb_change_cnt == 0)
	typebuf.tb_change_cnt = 1;
    return OK;
}

/*
 * Free the buffers of "typebuf".
 */
    void
free_typebuf(void)
{
    if (typebuf.tb_buf == typebuf_init)
	EMSG2(_(e_intern2), "Free typebuf 1");
    else
	vim_free(typebuf.tb_buf);
    if (typebuf.tb_noremap == noremapbuf_init)
	EMSG2(_(e_intern2), "Free typebuf 2");
    else
	vim_free(typebuf.tb_noremap);
}

/*
 * When doing ":so! file", the current typeahead needs to be saved, and
 * restored when "file" has been read completely.
 */
static typebuf_T saved_typebuf[NSCRIPT];

    int
save_typebuf(void)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    /* If out of memory: restore typebuf and close file. */
    if (alloc_typebuf() == FAIL)
    {
	closescript();
	return FAIL;
    }
    return OK;
}

static int old_char = -1;	/* character put back by vungetc() */
static int old_mod_mask;	/* mod_mask for ungotten character */
#ifdef FEAT_MOUSE
static int old_mouse_row;	/* mouse_row related to old_char */
static int old_mouse_col;	/* mouse_col related to old_char */
#endif

/*
 * Save all three kinds of typeahead, so that the user must type at a prompt.
 */
    void
save_typeahead(tasave_T *tp)
{
    tp->save_typebuf = typebuf;
    tp->typebuf_valid = (alloc_typebuf() == OK);
    if (!tp->typebuf_valid)
	typebuf = tp->save_typebuf;

    tp->old_char = old_char;
    tp->old_mod_mask = old_mod_mask;
    old_char = -1;

    tp->save_readbuf1 = readbuf1;
    readbuf1.bh_first.b_next = NULL;
    tp->save_readbuf2 = readbuf2;
    readbuf2.bh_first.b_next = NULL;
# ifdef USE_INPUT_BUF
    tp->save_inputbuf = get_input_buf();
# endif
}

/*
 * Restore the typeahead to what it was before calling save_typeahead().
 * The allocated memory is freed, can only be called once!
 */
    void
restore_typeahead(tasave_T *tp)
{
    if (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_mask = tp->old_mod_mask;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
# ifdef USE_INPUT_BUF
    set_input_buf(tp->save_inputbuf);
# endif
}

/*
 * Open a new script file for the ":source!" command.
 */
    void
openscript(
    char_u	*name,
    int		directly)	/* when TRUE execute directly */
{
    if (curscript + 1 == NSCRIPT)
    {
	EMSG(_(e_nesting));
	return;
    }
#ifdef FEAT_EVAL
    if (ignore_script)
	/* Not reading from script, also don't open one.  Warning message? */
	return;
#endif

    if (scriptin[curscript] != NULL)	/* already reading script */
	++curscript;
				/* use NameBuff for expanded name */
    expand_env(name, NameBuff, MAXPATHL);
    if ((scriptin[curscript] = mch_fopen((char *)NameBuff, READBIN)) == NULL)
    {
	EMSG2(_(e_notopen), name);
	if (curscript)
	    --curscript;
	return;
    }
    if (save_typebuf() == FAIL)
	return;

    /*
     * Execute the commands from the file right now when using ":source!"
     * after ":global" or ":argdo" or in a loop.  Also when another command
     * follows.  This means the display won't be updated.  Don't do this
     * always, "make test" would fail.
     */
    if (directly)
    {
	oparg_T	oa;
	int	oldcurscript;
	int	save_State = State;
	int	save_restart_edit = restart_edit;
	int	save_insertmode = p_im;
	int	save_finish_op = finish_op;
	int	save_msg_scroll = msg_scroll;

	State = NORMAL;
	msg_scroll = FALSE;	/* no msg scrolling in Normal mode */
	restart_edit = 0;	/* don't go to Insert mode */
	p_im = FALSE;		/* don't use 'insertmode' */
	clear_oparg(&oa);
	finish_op = FALSE;

	oldcurscript = curscript;
	do
	{
	    update_topline_cursor();	/* update cursor position and topline */
	    normal_cmd(&oa, FALSE);	/* execute one command */
	    vpeekc();			/* check for end of file */
	}
	while (scriptin[oldcurscript] != NULL);

	State = save_State;
	msg_scroll = save_msg_scroll;
	restart_edit = save_restart_edit;
	p_im = save_insertmode;
	finish_op = save_finish_op;
    }
}

/*
 * Close the currently active input script.
 */
    static void
closescript(void)
{
    free_typebuf();
    typebuf = saved_typebuf[curscript];

    fclose(scriptin[curscript]);
    scriptin[curscript] = NULL;
    if (curscript > 0)
	--curscript;
}

#if defined(EXITFREE) || defined(PROTO)
    void
close_all_scripts(void)
{
    while (scriptin[0] != NULL)
	closescript();
}
#endif

#if defined(FEAT_INS_EXPAND) || defined(PROTO)
/*
 * Return TRUE when reading keys from a script file.
 */
    int
using_script(void)
{
    return scriptin[curscript] != NULL;
}
#endif

/*
 * This function is called just before doing a blocking wait.  Thus after
 * waiting 'updatetime' for a character to arrive.
 */
    void
before_blocking(void)
{
    updatescript(0);
#ifdef FEAT_EVAL
    if (may_garbage_collect)
	garbage_collect();
#endif
}

/*
 * updatescipt() is called when a character can be written into the script file
 * or when we have waited some time for a character (c == 0)
 *
 * All the changed memfiles are synced if c == 0 or when the number of typed
 * characters reaches 'updatecount' and 'updatecount' is non-zero.
 */
    void
updatescript(int c)
{
    static int	    count = 0;

    if (c && scriptout)
	putc(c, scriptout);
    if (c == 0 || (p_uc > 0 && ++count >= p_uc))
    {
	ml_sync_all(c == 0, TRUE);
	count = 0;
    }
}

/*
 * Get the next input character.
 * Can return a special key or a multi-byte character.
 * Can return NUL when called recursively, use safe_vgetc() if that's not
 * wanted.
 * This translates escaped K_SPECIAL and CSI bytes to a K_SPECIAL or CSI byte.
 * Collects the bytes of a multibyte character into the whole character.
 * Returns the modifiers in the global "mod_mask".
 */
    int
vgetc(void)
{
    int		c, c2;
#ifdef FEAT_MBYTE
    int		n;
    char_u	buf[MB_MAXBYTES + 1];
    int		i;
#endif

#ifdef FEAT_EVAL
    /* Do garbage collection when garbagecollect() was called previously and
     * we are now at the toplevel. */
    if (may_garbage_collect && want_garbage_collect)
	garbage_collect();
#endif

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    if (old_char != -1)
    {
	c = old_char;
	old_char = -1;
	mod_mask = old_mod_mask;
#ifdef FEAT_MOUSE
	mouse_row = old_mouse_row;
	mouse_col = old_mouse_col;
#endif
    }
    else
    {
      mod_mask = 0x0;
      last_recorded_len = 0;
      for (;;)			/* this is done twice if there are modifiers */
      {
	int did_inc = FALSE;

	if (mod_mask)		/* no mapping after modifier has been read */
	{
	    ++no_mapping;
	    ++allow_keys;
	    did_inc = TRUE;	/* mod_mask may change value */
	}
	c = vgetorpeek(TRUE);
	if (did_inc)
	{
	    --no_mapping;
	    --allow_keys;
	}

	/* Get two extra bytes for special keys */
	if (c == K_SPECIAL
#ifdef FEAT_GUI
		|| c == CSI
#endif
	   )
	{
	    int	    save_allow_keys = allow_keys;

	    ++no_mapping;
	    allow_keys = 0;		/* make sure BS is not found */
	    c2 = vgetorpeek(TRUE);	/* no mapping for these chars */
	    c = vgetorpeek(TRUE);
	    --no_mapping;
	    allow_keys = save_allow_keys;
	    if (c2 == KS_MODIFIER)
	    {
		mod_mask = c;
		continue;
	    }
	    c = TO_SPECIAL(c2, c);

#if defined(FEAT_GUI_W32) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
	    /* Handle K_TEAROFF here, the caller of vgetc() doesn't need to
	     * know that a menu was torn off */
	    if (c == K_TEAROFF)
	    {
		char_u	name[200];
		int	i;

		/* get menu path, it ends with a <CR> */
		for (i = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		{
		    name[i] = c;
		    if (i < 199)
			++i;
		}
		name[i] = NUL;
		gui_make_tearoff(name);
		continue;
	    }
#endif
#if defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && defined(FEAT_MENU)
	    /* GTK: <F10> normally selects the menu, but it's passed until
	     * here to allow mapping it.  Intercept and invoke the GTK
	     * behavior if it's not mapped. */
	    if (c == K_F10 && gui.menubar != NULL)
	    {
		gtk_menu_shell_select_first(GTK_MENU_SHELL(gui.menubar), FALSE);
		continue;
	    }
#endif
#ifdef FEAT_GUI
	    /* Handle focus event here, so that the caller doesn't need to
	     * know about it.  Return K_IGNORE so that we loop once (needed if
	     * 'lazyredraw' is set). */
	    if (c == K_FOCUSGAINED || c == K_FOCUSLOST)
	    {
		ui_focus_change(c == K_FOCUSGAINED);
		c = K_IGNORE;
	    }

	    /* Translate K_CSI to CSI.  The special key is only used to avoid
	     * it being recognized as the start of a special key. */
	    if (c == K_CSI)
		c = CSI;
#endif
	}
	/* a keypad or special function key was not mapped, use it like
	 * its ASCII equivalent */
	switch (c)
	{
	    case K_KPLUS:		c = '+'; break;
	    case K_KMINUS:		c = '-'; break;
	    case K_KDIVIDE:		c = '/'; break;
	    case K_KMULTIPLY:	c = '*'; break;
	    case K_KENTER:		c = CAR; break;
	    case K_KPOINT:
#ifdef WIN32
				    /* Can be either '.' or a ',', *
				     * depending on the type of keypad. */
				    c = MapVirtualKey(VK_DECIMAL, 2); break;
#else
				    c = '.'; break;
#endif
	    case K_K0:		c = '0'; break;
	    case K_K1:		c = '1'; break;
	    case K_K2:		c = '2'; break;
	    case K_K3:		c = '3'; break;
	    case K_K4:		c = '4'; break;
	    case K_K5:		c = '5'; break;
	    case K_K6:		c = '6'; break;
	    case K_K7:		c = '7'; break;
	    case K_K8:		c = '8'; break;
	    case K_K9:		c = '9'; break;

	    case K_XHOME:
	    case K_ZHOME:	if (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_HOME;
				    mod_mask = 0;
				}
				else if (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_HOME;
				    mod_mask = 0;
				}
				else
				    c = K_HOME;
				break;
	    case K_XEND:
	    case K_ZEND:	if (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_END;
				    mod_mask = 0;
				}
				else if (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_END;
				    mod_mask = 0;
				}
				else
				    c = K_END;
				break;

	    case K_XUP:		c = K_UP; break;
	    case K_XDOWN:	c = K_DOWN; break;
	    case K_XLEFT:	c = K_LEFT; break;
	    case K_XRIGHT:	c = K_RIGHT; break;
	}

#ifdef FEAT_MBYTE
	/* For a multi-byte character get all the bytes and return the
	 * converted character.
	 * Note: This will loop until enough bytes are received!
	 */
	if (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	{
	    ++no_mapping;
	    buf[0] = c;
	    for (i = 1; i < n; ++i)
	    {
		buf[i] = vgetorpeek(TRUE);
		if (buf[i] == K_SPECIAL
#ifdef FEAT_GUI
			|| buf[i] == CSI
#endif
			)
		{
		    /* Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER sequence,
		     * which represents a K_SPECIAL (0x80),
		     * or a CSI - KS_EXTRA - KE_CSI sequence, which represents
		     * a CSI (0x9B),
		     * of a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI too. */
		    c = vgetorpeek(TRUE);
		    if (vgetorpeek(TRUE) == (int)KE_CSI && c == KS_EXTRA)
			buf[i] = CSI;
		}
	    }
	    --no_mapping;
	    c = (*mb_ptr2char)(buf);
	}
#endif

	break;
      }
    }

#ifdef FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#endif

    return c;
}

/*
 * Like vgetc(), but never return a NUL when called recursively, get a key
 * directly from the user (ignoring typeahead).
 */
    int
safe_vgetc(void)
{
    int	c;

    c = vgetc();
    if (c == NUL)
	c = get_keystroke();
    return c;
}

/*
 * Like safe_vgetc(), but loop to handle K_IGNORE.
 * Also ignore scrollbar events.
 */
    int
plain_vgetc(void)
{
    int c;

    do
    {
	c = safe_vgetc();
    } while (c == K_IGNORE || c == K_VER_SCROLLBAR || c == K_HOR_SCROLLBAR);
    return c;
}

/*
 * Check if a character is available, such that vgetc() will not block.
 * If the next character is a special character or multi-byte, the returned
 * character is not valid!.
 */
    int
vpeekc(void)
{
    if (old_char != -1)
	return old_char;
    return vgetorpeek(FALSE);
}

#if defined(FEAT_TERMRESPONSE) || defined(PROTO)
/*
 * Like vpeekc(), but don't allow mapping.  Do allow checking for terminal
 * codes.
 */
    int
vpeekc_nomap(void)
{
    int		c;

    ++no_mapping;
    ++allow_keys;
    c = vpeekc();
    --no_mapping;
    --allow_keys;
    return c;
}
#endif

#if defined(FEAT_INS_EXPAND) || defined(FEAT_EVAL) || defined(PROTO)
/*
 * Check if any character is available, also half an escape sequence.
 * Trick: when no typeahead found, but there is something in the typeahead
 * buffer, it must be an ESC that is recognized as the start of a key code.
 */
    int
vpeekc_any(void)
{
    int		c;

    c = vpeekc();
    if (c == NUL && typebuf.tb_len > 0)
	c = ESC;
    return c;
}
#endif

/*
 * Call vpeekc() without causing anything to be mapped.
 * Return TRUE if a character is available, FALSE otherwise.
 */
    int
char_avail(void)
{
    int	    retval;

#ifdef FEAT_EVAL
    /* When disable_char_avail_for_testing(1) was called pretend there is no
     * typeahead. */
    if (disable_char_avail_for_testing)
	return FALSE;
#endif
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    return (retval != NUL);
}

/*
 * unget one character (can only be done once!)
 */
    void
vungetc(int c)
{
    old_char = c;
    old_mod_mask = mod_mask;
#ifdef FEAT_MOUSE
    old_mouse_row = mouse_row;
    old_mouse_col = mouse_col;
#endif
}

/*
 * get a character:
 * 1. from the stuffbuffer
 *	This is used for abbreviated commands like "D" -> "d$".
 *	Also used to redo a command for ".".
 * 2. from the typeahead buffer
 *	Stores text obtained previously but not used yet.
 *	Also stores the result of mappings.
 *	Also used for the ":normal" command.
 * 3. from the user
 *	This may do a blocking wait if "advance" is TRUE.
 *
 * if "advance" is TRUE (vgetc()):
 *	really get the character.
 *	KeyTyped is set to TRUE in the case the user typed the key.
 *	KeyStuffed is TRUE if the character comes from the stuff buffer.
 * if "advance" is FALSE (vpeekc()):
 *	just look whether there is a character available.
 *
 * When "no_mapping" is zero, checks for mappings in the current mode.
 * Only returns one byte (of a multi-byte character).
 * K_SPECIAL and CSI may be escaped, need to get two more bytes then.
 */
    static int
vgetorpeek(int advance)
{
    int		c, c1;
    int		keylen;
    char_u	*s;
    mapblock_T	*mp;
#ifdef FEAT_LOCALMAP
    mapblock_T	*mp2;
#endif
    mapblock_T	*mp_match;
    int		mp_match_len = 0;
    int		timedout = FALSE;	    /* waited for more than 1 second
						for mapping to complete */
    int		mapdepth = 0;	    /* check for recursive mapping */
    int		mode_deleted = FALSE;   /* set when mode has been deleted */
    int		local_State;
    int		mlen;
    int		max_mlen;
    int		i;
#ifdef FEAT_CMDL_INFO
    int		new_wcol, new_wrow;
#endif
#ifdef FEAT_GUI
# ifdef FEAT_MENU
    int		idx;
# endif
    int		shape_changed = FALSE;  /* adjusted cursor shape */
#endif
    int		n;
#ifdef FEAT_LANGMAP
    int		nolmaplen;
#endif
    int		old_wcol, old_wrow;
    int		wait_tb_len;

    /*
     * This function doesn't work very well when called recursively.  This may
     * happen though, because of:
     * 1. The call to add_to_showcmd().	char_avail() is then used to check if
     * there is a character available, which calls this function.  In that
     * case we must return NUL, to indicate no character is available.
     * 2. A GUI callback function writes to the screen, causing a
     * wait_return().
     * Using ":normal" can also do this, but it saves the typeahead buffer,
     * thus it should be OK.  But don't get a key from the user then.
     */
    if (vgetc_busy > 0 && ex_normal_busy == 0)
	return NUL;

    local_State = get_real_state();

    ++vgetc_busy;

    if (advance)
	KeyStuffed = FALSE;

    init_typebuf();
    start_stuff();
    if (advance && typebuf.tb_maplen == 0)
	Exec_reg = FALSE;
    do
    {
/*
 * get a character: 1. from the stuffbuffer
 */
	if (typeahead_char != 0)
	{
	    c = typeahead_char;
	    if (advance)
		typeahead_char = 0;
	}
	else
	    c = read_readbuffers(advance);
	if (c != NUL && !got_int)
	{
	    if (advance)
	    {
		/* KeyTyped = FALSE;  When the command that stuffed something
		 * was typed, behave like the stuffed command was typed.
		 * needed for CTRL-W CTRl-] to open a fold, for example. */
		KeyStuffed = TRUE;
	    }
	    if (typebuf.tb_no_abbr_cnt == 0)
		typebuf.tb_no_abbr_cnt = 1;	/* no abbreviations now */
	}
	else
	{
	    /*
	     * Loop until we either find a matching mapped key, or we
	     * are sure that it is not a mapped key.
	     * If a mapped key sequence is found we go back to the start to
	     * try re-mapping.
	     */
	    for (;;)
	    {
		/*
		 * ui_breakcheck() is slow, don't use it too often when
		 * inside a mapping.  But call it each time for typed
		 * characters.
		 */
		if (typebuf.tb_maplen)
		    line_breakcheck();
		else
		    ui_breakcheck();		/* check for CTRL-C */
		keylen = 0;
		if (got_int)
		{
		    /* flush all input */
		    c = inchar(typebuf.tb_buf, typebuf.tb_buflen - 1, 0L,
						       typebuf.tb_change_cnt);
		    /*
		     * If inchar() returns TRUE (script file was active) or we
		     * are inside a mapping, get out of insert mode.
		     * Otherwise we behave like having gotten a CTRL-C.
		     * As a result typing CTRL-C in insert mode will
		     * really insert a CTRL-C.
		     */
		    if ((c || typebuf.tb_maplen)
					      && (State & (INSERT + CMDLINE)))
			c = ESC;
		    else
			c = Ctrl_C;
		    flush_buffers(TRUE);	/* flush all typeahead */

		    if (advance)
		    {
			/* Also record this character, it might be needed to
			 * get out of Insert mode. */
			*typebuf.tb_buf = c;
			gotchars(typebuf.tb_buf, 1);
		    }
		    cmd_silent = FALSE;

		    break;
		}
		else if (typebuf.tb_len > 0)
		{
		    /*
		     * Check for a mappable key sequence.
		     * Walk through one maphash[] list until we find an
		     * entry that matches.
		     *
		     * Don't look for mappings if:
		     * - no_mapping set: mapping disabled (e.g. for CTRL-V)
		     * - maphash_valid not set: no mappings present.
		     * - typebuf.tb_buf[typebuf.tb_off] should not be remapped
		     * - in insert or cmdline mode and 'paste' option set
		     * - waiting for "hit return to continue" and CR or SPACE
		     *	 typed
		     * - waiting for a char with --more--
		     * - in Ctrl-X mode, and we get a valid char for that mode
		     */
		    mp = NULL;
		    max_mlen = 0;
		    c1 = typebuf.tb_buf[typebuf.tb_off];
		    if (no_mapping == 0 && maphash_valid
			    && (no_zero_mapping == 0 || c1 != '0')
			    && (typebuf.tb_maplen == 0
				|| (p_remap
				    && (typebuf.tb_noremap[typebuf.tb_off]
						    & (RM_NONE|RM_ABBR)) == 0))
			    && !(p_paste && (State & (INSERT + CMDLINE)))
			    && !(State == HITRETURN && (c1 == CAR || c1 == ' '))
			    && State != ASKMORE
			    && State != CONFIRM
#ifdef FEAT_INS_EXPAND
			    && !((ctrl_x_mode != 0 && vim_is_ctrl_x_key(c1))
				    || ((compl_cont_status & CONT_LOCAL)
					&& (c1 == Ctrl_N || c1 == Ctrl_P)))
#endif
			    )
		    {
#ifdef FEAT_LANGMAP
			if (c1 == K_SPECIAL)
			    nolmaplen = 2;
			else
			{
			    LANGMAP_ADJUST(c1,
					   (State & (CMDLINE | INSERT)) == 0
					   && get_real_state() != SELECTMODE);
			    nolmaplen = 0;
			}
#endif
#ifdef FEAT_LOCALMAP
			/* First try buffer-local mappings. */
			mp = curbuf->b_maphash[MAP_HASH(local_State, c1)];
			mp2 = maphash[MAP_HASH(local_State, c1)];
			if (mp == NULL)
			{
			    /* There are no buffer-local mappings. */
			    mp = mp2;
			    mp2 = NULL;
			}
#else
			mp = maphash[MAP_HASH(local_State, c1)];
#endif
			/*
			 * Loop until a partly matching mapping is found or
			 * all (local) mappings have been checked.
			 * The longest full match is remembered in "mp_match".
			 * A full match is only accepted if there is no partly
			 * match, so "aa" and "aaa" can both be mapped.
			 */
			mp_match = NULL;
			mp_match_len = 0;
			for ( ; mp != NULL;
#ifdef FEAT_LOCALMAP
				mp->m_next == NULL ? (mp = mp2, mp2 = NULL) :
#endif
				(mp = mp->m_next))
			{
			    /*
			     * Only consider an entry if the first character
			     * matches and it is for the current state.
			     * Skip ":lmap" mappings if keys were mapped.
			     */
			    if (mp->m_keys[0] == c1
				    && (mp->m_mode & local_State)
				    && ((mp->m_mode & LANGMAP) == 0
					|| typebuf.tb_maplen == 0))
			    {
#ifdef FEAT_LANGMAP
				int	nomap = nolmaplen;
				int	c2;
#endif
				/* find the match length of this mapping */
				for (mlen = 1; mlen < typebuf.tb_len; ++mlen)
				{
#ifdef FEAT_LANGMAP
				    c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
				    if (nomap > 0)
					--nomap;
				    else if (c2 == K_SPECIAL)
					nomap = 2;
				    else
					LANGMAP_ADJUST(c2, TRUE);
				    if (mp->m_keys[mlen] != c2)
#else
				    if (mp->m_keys[mlen] !=
					typebuf.tb_buf[typebuf.tb_off + mlen])
#endif
					break;
				}

#ifdef FEAT_MBYTE
				/* Don't allow mapping the first byte(s) of a
				 * multi-byte char.  Happens when mapping
				 * <M-a> and then changing 'encoding'. Beware
				 * that 0x80 is escaped. */
				{
				    char_u *p1 = mp->m_keys;
				    char_u *p2 = mb_unescape(&p1);

				    if (has_mbyte && p2 != NULL
					  && MB_BYTE2LEN(c1) > MB_PTR2LEN(p2))
					mlen = 0;
				}
#endif
				/*
				 * Check an entry whether it matches.
				 * - Full match: mlen == keylen
				 * - Partly match: mlen == typebuf.tb_len
				 */
				keylen = mp->m_keylen;
				if (mlen == keylen
				     || (mlen == typebuf.tb_len
						  && typebuf.tb_len < keylen))
				{
				    /*
				     * If only script-local mappings are
				     * allowed, check if the mapping starts
				     * with K_SNR.
				     */
				    s = typebuf.tb_noremap + typebuf.tb_off;
				    if (*s == RM_SCRIPT
					    && (mp->m_keys[0] != K_SPECIAL
						|| mp->m_keys[1] != KS_EXTRA
						|| mp->m_keys[2]
							      != (int)KE_SNR))
					continue;
				    /*
				     * If one of the typed keys cannot be
				     * remapped, skip the entry.
				     */
				    for (n = mlen; --n >= 0; )
					if (*s++ & (RM_NONE|RM_ABBR))
					    break;
				    if (n >= 0)
					continue;

				    if (keylen > typebuf.tb_len)
				    {
					if (!timedout && !(mp_match != NULL
						       && mp_match->m_nowait))
					{
					    /* break at a partly match */
					    keylen = KEYLEN_PART_MAP;
					    break;
					}
				    }
				    else if (keylen > mp_match_len)
				    {
					/* found a longer match */
					mp_match = mp;
					mp_match_len = keylen;
				    }
				}
				else
				    /* No match; may have to check for
				     * termcode at next character. */
				    if (max_mlen < mlen)
					max_mlen = mlen;
			    }
			}

			/* If no partly match found, use the longest full
			 * match. */
			if (keylen != KEYLEN_PART_MAP)
			{
			    mp = mp_match;
			    keylen = mp_match_len;
			}
		    }

		    /* Check for match with 'pastetoggle' */
		    if (*p_pt != NUL && mp == NULL && (State & (INSERT|NORMAL)))
		    {
			for (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen];
								       ++mlen)
			    if (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off
								      + mlen])
				    break;
			if (p_pt[mlen] == NUL)	/* match */
			{
			    /* write chars to script file(s) */
			    if (mlen > typebuf.tb_maplen)
				gotchars(typebuf.tb_buf + typebuf.tb_off
							  + typebuf.tb_maplen,
						    mlen - typebuf.tb_maplen);

			    del_typebuf(mlen, 0); /* remove the chars */
			    set_option_value((char_u *)"paste",
						     (long)!p_paste, NULL, 0);
			    if (!(State & INSERT))
			    {
				msg_col = 0;
				msg_row = Rows - 1;
				msg_clr_eos();		/* clear ruler */
			    }
#ifdef FEAT_WINDOWS
			    status_redraw_all();
			    redraw_statuslines();
#endif
			    showmode();
			    setcursor();
			    continue;
			}
			/* Need more chars for partly match. */
			if (mlen == typebuf.tb_len)
			    keylen = KEYLEN_PART_KEY;
			else if (max_mlen < mlen)
			    /* no match, may have to check for termcode at
			     * next character */
			    max_mlen = mlen + 1;
		    }

		    if ((mp == NULL || max_mlen >= mp_match_len)
						 && keylen != KEYLEN_PART_MAP)
		    {
			int	save_keylen = keylen;

			/*
			 * When no matching mapping found or found a
			 * non-matching mapping that matches at least what the
			 * matching mapping matched:
			 * Check if we have a terminal code, when:
			 *  mapping is allowed,
			 *  keys have not been mapped,
			 *  and not an ESC sequence, not in insert mode or
			 *	p_ek is on,
			 *  and when not timed out,
			 */
			if ((no_mapping == 0 || allow_keys != 0)
				&& (typebuf.tb_maplen == 0
				    || (p_remap && typebuf.tb_noremap[
						   typebuf.tb_off] == RM_YES))
				&& !timedout)
			{
			    keylen = check_termcode(max_mlen + 1,
							       NULL, 0, NULL);

			    /* If no termcode matched but 'pastetoggle'
			     * matched partially it's like an incomplete key
			     * sequence. */
			    if (keylen == 0 && save_keylen == KEYLEN_PART_KEY)
				keylen = KEYLEN_PART_KEY;

			    /*
			     * When getting a partial match, but the last
			     * characters were not typed, don't wait for a
			     * typed character to complete the termcode.
			     * This helps a lot when a ":normal" command ends
			     * in an ESC.
			     */
			    if (keylen < 0
				       && typebuf.tb_len == typebuf.tb_maplen)
				keylen = 0;
			}
			else
			    keylen = 0;
			if (keylen == 0)	/* no matching terminal code */
			{
#ifdef AMIGA			/* check for window bounds report */
			    if (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
					       typebuf.tb_off] & 0xff) == CSI)
			    {
				for (s = typebuf.tb_buf + typebuf.tb_off + 1;
					s < typebuf.tb_buf + typebuf.tb_off
							      + typebuf.tb_len
				   && (VIM_ISDIGIT(*s) || *s == ';'
								|| *s == ' ');
					++s)
				    ;
				if (*s == 'r' || *s == '|') /* found one */
				{
				    del_typebuf((int)(s + 1 -
				       (typebuf.tb_buf + typebuf.tb_off)), 0);
				    /* get size and redraw screen */
				    shell_resized();
				    continue;
				}
				if (*s == NUL)	    /* need more characters */
				    keylen = KEYLEN_PART_KEY;
			    }
			    if (keylen >= 0)
#endif
			      /* When there was a matching mapping and no
			       * termcode could be replaced after another one,
			       * use that mapping (loop around). If there was
			       * no mapping use the character from the
			       * typeahead buffer right here. */
			      if (mp == NULL)
			      {
/*
 * get a character: 2. from the typeahead buffer
 */
				c = typebuf.tb_buf[typebuf.tb_off] & 255;
				if (advance)	/* remove chars from tb_buf */
				{
				    cmd_silent = (typebuf.tb_silent > 0);
				    if (typebuf.tb_maplen > 0)
					KeyTyped = FALSE;
				    else
				    {
					KeyTyped = TRUE;
					/* write char to script file(s) */
					gotchars(typebuf.tb_buf
							 + typebuf.tb_off, 1);
				    }
				    KeyNoremap = typebuf.tb_noremap[
							      typebuf.tb_off];
				    del_typebuf(1, 0);
				}
				break;	    /* got character, break for loop */
			      }
			}
			if (keylen > 0)	    /* full matching terminal code */
			{
#if defined(FEAT_GUI) && defined(FEAT_MENU)
			    if (typebuf.tb_len >= 2
				&& typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
					 && typebuf.tb_buf[typebuf.tb_off + 1]
								   == KS_MENU)
			    {
				/*
				 * Using a menu may cause a break in undo!
				 * It's like using gotchars(), but without
				 * recording or writing to a script file.
				 */
				may_sync_undo();
				del_typebuf(3, 0);
				idx = get_menu_index(current_menu, local_State);
				if (idx != MENU_INDEX_INVALID)
				{
				    /*
				     * In Select mode and a Visual mode menu
				     * is used:  Switch to Visual mode
				     * temporarily.  Append K_SELECT to switch
				     * back to Select mode.
				     */
				    if (VIsual_active && VIsual_select
					    && (current_menu->modes & VISUAL))
				    {
					VIsual_select = FALSE;
					(void)ins_typebuf(K_SELECT_STRING,
						  REMAP_NONE, 0, TRUE, FALSE);
				    }
				    ins_typebuf(current_menu->strings[idx],
						current_menu->noremap[idx],
						0, TRUE,
						   current_menu->silent[idx]);
				}
			    }
#endif /* FEAT_GUI && FEAT_MENU */
			    continue;	/* try mapping again */
			}

			/* Partial match: get some more characters.  When a
			 * matching mapping was found use that one. */
			if (mp == NULL || keylen < 0)
			    keylen = KEYLEN_PART_KEY;
			else
			    keylen = mp_match_len;
		    }

		    /* complete match */
		    if (keylen >= 0 && keylen <= typebuf.tb_len)
		    {
#ifdef FEAT_EVAL
			int save_m_expr;
			int save_m_noremap;
			int save_m_silent;
			char_u *save_m_keys;
			char_u *save_m_str;
#else
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

			/* write chars to script file(s) */
			if (keylen > typebuf.tb_maplen)
			    gotchars(typebuf.tb_buf + typebuf.tb_off
							  + typebuf.tb_maplen,
						  keylen - typebuf.tb_maplen);

			cmd_silent = (typebuf.tb_silent > 0);
			del_typebuf(keylen, 0);	/* remove the mapped keys */

			/*
			 * Put the replacement string in front of mapstr.
			 * The depth check catches ":map x y" and ":map y x".
			 */
			if (++mapdepth >= p_mmd)
			{
			    EMSG(_("E223: recursive mapping"));
			    if (State & CMDLINE)
				redrawcmdline();
			    else
				setcursor();
			    flush_buffers(FALSE);
			    mapdepth = 0;	/* for next one */
			    c = -1;
			    break;
			}

			/*
			 * In Select mode and a Visual mode mapping is used:
			 * Switch to Visual mode temporarily.  Append K_SELECT
			 * to switch back to Select mode.
			 */
			if (VIsual_active && VIsual_select
						     && (mp->m_mode & VISUAL))
			{
			    VIsual_select = FALSE;
			    (void)ins_typebuf(K_SELECT_STRING, REMAP_NONE,
							      0, TRUE, FALSE);
			}

#ifdef FEAT_EVAL
			/* Copy the values from *mp that are used, because
			 * evaluating the expression may invoke a function
			 * that redefines the mapping, thereby making *mp
			 * invalid. */
			save_m_expr = mp->m_expr;
			save_m_noremap = mp->m_noremap;
			save_m_silent = mp->m_silent;
			save_m_keys = NULL;  /* only saved when needed */
			save_m_str = NULL;  /* only saved when needed */

			/*
			 * Handle ":map <expr>": evaluate the {rhs} as an
			 * expression.  Also save and restore the command line
			 * for "normal :".
			 */
			if (mp->m_expr)
			{
			    int		save_vgetc_busy = vgetc_busy;

			    vgetc_busy = 0;
			    save_m_keys = vim_strsave(mp->m_keys);
			    save_m_str = vim_strsave(mp->m_str);
			    s = eval_map_expr(save_m_str, NUL);
			    vgetc_busy = save_vgetc_busy;
			}
			else
#endif
			    s = mp->m_str;

			/*
			 * Insert the 'to' part in the typebuf.tb_buf.
			 * If 'from' field is the same as the start of the
			 * 'to' field, don't remap the first character (but do
			 * allow abbreviations).
			 * If m_noremap is set, don't remap the whole 'to'
			 * part.
			 */
			if (s == NULL)
			    i = FAIL;
			else
			{
			    int noremap;

			    if (save_m_noremap != REMAP_YES)
				noremap = save_m_noremap;
			    else if (
#ifdef FEAT_EVAL
				STRNCMP(s, save_m_keys != NULL
						   ? save_m_keys : mp->m_keys,
							 (size_t)keylen)
#else
				STRNCMP(s, mp->m_keys, (size_t)keylen)
#endif
				   != 0)
				noremap = REMAP_YES;
			    else
				noremap = REMAP_SKIP;
			    i = ins_typebuf(s, noremap,
					0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
			    if (save_m_expr)
				vim_free(s);
#endif
			}
#ifdef FEAT_EVAL
			vim_free(save_m_keys);
			vim_free(save_m_str);
#endif
			if (i == FAIL)
			{
			    c = -1;
			    break;
			}
			continue;
		    }
		}

/*
 * get a character: 3. from the user - handle <Esc> in Insert mode
 */
		/*
		 * special case: if we get an <ESC> in insert mode and there
		 * are no more characters at once, we pretend to go out of
		 * insert mode.  This prevents the one second delay after
		 * typing an <ESC>.  If we get something after all, we may
		 * have to redisplay the mode. That the cursor is in the wrong
		 * place does not matter.
		 */
		c = 0;
#ifdef FEAT_CMDL_INFO
		new_wcol = curwin->w_wcol;
		new_wrow = curwin->w_wrow;
#endif
		if (	   advance
			&& typebuf.tb_len == 1
			&& typebuf.tb_buf[typebuf.tb_off] == ESC
			&& !no_mapping
			&& ex_normal_busy == 0
			&& typebuf.tb_maplen == 0
			&& (State & INSERT)
			&& (p_timeout
			    || (keylen == KEYLEN_PART_KEY && p_ttimeout))
			&& (c = inchar(typebuf.tb_buf + typebuf.tb_off
						     + typebuf.tb_len, 3, 25L,
						 typebuf.tb_change_cnt)) == 0)
		{
		    colnr_T	col = 0, vcol;
		    char_u	*ptr;

		    if (mode_displayed)
		    {
			unshowmode(TRUE);
			mode_deleted = TRUE;
		    }
#ifdef FEAT_GUI
		    /* may show different cursor shape */
		    if (gui.in_use)
		    {
			int	    save_State;

			save_State = State;
			State = NORMAL;
			gui_update_cursor(TRUE, FALSE);
			State = save_State;
			shape_changed = TRUE;
		    }
#endif
		    validate_cursor();
		    old_wcol = curwin->w_wcol;
		    old_wrow = curwin->w_wrow;

		    /* move cursor left, if possible */
		    if (curwin->w_cursor.col != 0)
		    {
			if (curwin->w_wcol > 0)
			{
			    if (did_ai)
			    {
				/*
				 * We are expecting to truncate the trailing
				 * white-space, so find the last non-white
				 * character -- webb
				 */
				col = vcol = curwin->w_wcol = 0;
				ptr = ml_get_curline();
				while (col < curwin->w_cursor.col)
				{
				    if (!vim_iswhite(ptr[col]))
					curwin->w_wcol = vcol;
				    vcol += lbr_chartabsize(ptr, ptr + col,
							       (colnr_T)vcol);
#ifdef FEAT_MBYTE
				    if (has_mbyte)
					col += (*mb_ptr2len)(ptr + col);
				    else
#endif
					++col;
				}
				curwin->w_wrow = curwin->w_cline_row
					   + curwin->w_wcol / W_WIDTH(curwin);
				curwin->w_wcol %= W_WIDTH(curwin);
				curwin->w_wcol += curwin_col_off();
#ifdef FEAT_MBYTE
				col = 0;	/* no correction needed */
#endif
			    }
			    else
			    {
				--curwin->w_wcol;
#ifdef FEAT_MBYTE
				col = curwin->w_cursor.col - 1;
#endif
			    }
			}
			else if (curwin->w_p_wrap && curwin->w_wrow)
			{
			    --curwin->w_wrow;
			    curwin->w_wcol = W_WIDTH(curwin) - 1;
#ifdef FEAT_MBYTE
			    col = curwin->w_cursor.col - 1;
#endif
			}
#ifdef FEAT_MBYTE
			if (has_mbyte && col > 0 && curwin->w_wcol > 0)
			{
			    /* Correct when the cursor is on the right halve
			     * of a double-wide character. */
			    ptr = ml_get_curline();
			    col -= (*mb_head_off)(ptr, ptr + col);
			    if ((*mb_ptr2cells)(ptr + col) > 1)
				--curwin->w_wcol;
			}
#endif
		    }
		    setcursor();
		    out_flush();
#ifdef FEAT_CMDL_INFO
		    new_wcol = curwin->w_wcol;
		    new_wrow = curwin->w_wrow;
#endif
		    curwin->w_wcol = old_wcol;
		    curwin->w_wrow = old_wrow;
		}
		if (c < 0)
		    continue;	/* end of input script reached */

		/* Allow mapping for just typed characters. When we get here c
		 * is the number of extra bytes and typebuf.tb_len is 1. */
		for (n = 1; n <= c; ++n)
		    typebuf.tb_noremap[typebuf.tb_off + n] = RM_YES;
		typebuf.tb_len += c;

		/* buffer full, don't map */
		if (typebuf.tb_len >= typebuf.tb_maplen + MAXMAPLEN)
		{
		    timedout = TRUE;
		    continue;
		}

		if (ex_normal_busy > 0)
		{
#ifdef FEAT_CMDWIN
		    static int tc = 0;
#endif

		    /* No typeahead left and inside ":normal".  Must return
		     * something to avoid getting stuck.  When an incomplete
		     * mapping is present, behave like it timed out. */
		    if (typebuf.tb_len > 0)
		    {
			timedout = TRUE;
			continue;
		    }
		    /* When 'insertmode' is set, ESC just beeps in Insert
		     * mode.  Use CTRL-L to make edit() return.
		     * For the command line only CTRL-C always breaks it.
		     * For the cmdline window: Alternate between ESC and
		     * CTRL-C: ESC for most situations and CTRL-C to close the
		     * cmdline window. */
		    if (p_im && (State & INSERT))
			c = Ctrl_L;
		    else if ((State & CMDLINE)
#ifdef FEAT_CMDWIN
			    || (cmdwin_type > 0 && tc == ESC)
#endif
			    )
			c = Ctrl_C;
		    else
			c = ESC;
#ifdef FEAT_CMDWIN
		    tc = c;
#endif
		    break;
		}

/*
 * get a character: 3. from the user - update display
 */
		/* In insert mode a screen update is skipped when characters
		 * are still available.  But when those available characters
		 * are part of a mapping, and we are going to do a blocking
		 * wait here.  Need to update the screen to display the
		 * changed text so far. Also for when 'lazyredraw' is set and
		 * redrawing was postponed because there was something in the
		 * input buffer (e.g., termresponse). */
		if (((State & INSERT) != 0 || p_lz) && (State & CMDLINE) == 0
			  && advance && must_redraw != 0 && !need_wait_return)
		{
		    update_screen(0);
		    setcursor(); /* put cursor back where it belongs */
		}

		/*
		 * If we have a partial match (and are going to wait for more
		 * input from the user), show the partially matched characters
		 * to the user with showcmd.
		 */
#ifdef FEAT_CMDL_INFO
		i = 0;
#endif
		c1 = 0;
		if (typebuf.tb_len > 0 && advance && !exmode_active)
		{
		    if (((State & (NORMAL | INSERT)) || State == LANGMAP)
			    && State != HITRETURN)
		    {
			/* this looks nice when typing a dead character map */
			if (State & INSERT
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
			{
			    edit_putchar(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			    setcursor(); /* put cursor back where it belongs */
			    c1 = 1;
			}
#ifdef FEAT_CMDL_INFO
			/* need to use the col and row from above here */
			old_wcol = curwin->w_wcol;
			old_wrow = curwin->w_wrow;
			curwin->w_wcol = new_wcol;
			curwin->w_wrow = new_wrow;
			push_showcmd();
			if (typebuf.tb_len > SHOWCMD_COLS)
			    i = typebuf.tb_len - SHOWCMD_COLS;
			while (i < typebuf.tb_len)
			    (void)add_to_showcmd(typebuf.tb_buf[typebuf.tb_off
								      + i++]);
			curwin->w_wcol = old_wcol;
			curwin->w_wrow = old_wrow;
#endif
		    }

		    /* this looks nice when typing a dead character map */
		    if ((State & CMDLINE)
#if defined(FEAT_CRYPT) || defined(FEAT_EVAL)
			    && cmdline_star == 0
#endif
			    && ptr2cells(typebuf.tb_buf + typebuf.tb_off
						   + typebuf.tb_len - 1) == 1)
		    {
			putcmdline(typebuf.tb_buf[typebuf.tb_off
						+ typebuf.tb_len - 1], FALSE);
			c1 = 1;
		    }
		}

/*
 * get a character: 3. from the user - get it
 */
		wait_tb_len = typebuf.tb_len;
		c = inchar(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len,
			typebuf.tb_buflen - typebuf.tb_off - typebuf.tb_len - 1,
			!advance
			    ? 0
			    : ((typebuf.tb_len == 0
				    || !(p_timeout || (p_ttimeout
					       && keylen == KEYLEN_PART_KEY)))
				    ? -1L
				    : ((keylen == KEYLEN_PART_KEY && p_ttm >= 0)
					    ? p_ttm
					    : p_tm)), typebuf.tb_change_cnt);

#ifdef FEAT_CMDL_INFO
		if (i != 0)
		    pop_showcmd();
#endif
		if (c1 == 1)
		{
		    if (State & INSERT)
			edit_unputchar();
		    if (State & CMDLINE)
			unputcmdline();
		    else
			setcursor();	/* put cursor back where it belongs */
		}

		if (c < 0)
		    continue;		/* end of input script reached */
		if (c == NUL)		/* no character available */
		{
		    if (!advance)
			break;
		    if (wait_tb_len > 0)	/* timed out */
		    {
			timedout = TRUE;
			continue;
		    }
		}
		else
		{	    /* allow mapping for just typed characters */
		    while (typebuf.tb_buf[typebuf.tb_off
						     + typebuf.tb_len] != NUL)
			typebuf.tb_noremap[typebuf.tb_off
						 + typebuf.tb_len++] = RM_YES;
#ifdef USE_IM_CONTROL
		    /* Get IM status right after getting keys, not after the
		     * timeout for a mapping (focus may be lost by then). */
		    vgetc_im_active = im_get_status();
#endif
		}
	    }	    /* for (;;) */
	}	/* if (!character from stuffbuf) */

			/* if advance is FALSE don't loop on NULs */
    } while (c < 0 || (advance && c == NUL));

    /*
     * The "INSERT" message is taken care of here:
     *	 if we return an ESC to exit insert mode, the message is deleted
     *	 if we don't return an ESC but deleted the message before, redisplay it
     */
    if (advance && p_smd && msg_silent == 0 && (State & INSERT))
    {
	if (c == ESC && !mode_deleted && !no_mapping && mode_displayed)
	{
	    if (typebuf.tb_len && !KeyTyped)
		redraw_cmdline = TRUE;	    /* delete mode later */
	    else
		unshowmode(FALSE);
	}
	else if (c != ESC && mode_deleted)
	{
	    if (typebuf.tb_len && !KeyTyped)
		redraw_cmdline = TRUE;	    /* show mode later */
	    else
		showmode();
	}
    }
#ifdef FEAT_GUI
    /* may unshow different cursor shape */
    if (gui.in_use && shape_changed)
	gui_update_cursor(TRUE, FALSE);
#endif

    --vgetc_busy;

    return c;
}

/*
 * inchar() - get one character from
 *	1. a scriptfile
 *	2. the keyboard
 *
 *  As much characters as we can get (upto 'maxlen') are put in "buf" and
 *  NUL terminated (buffer length must be 'maxlen' + 1).
 *  Minimum for "maxlen" is 3!!!!
 *
 *  "tb_change_cnt" is the value of typebuf.tb_change_cnt if "buf" points into
 *  it.  When typebuf.tb_change_cnt changes (e.g., when a message is received
 *  from a remote client) "buf" can no longer be used.  "tb_change_cnt" is 0
 *  otherwise.
 *
 *  If we got an interrupt all input is read until none is available.
 *
 *  If wait_time == 0  there is no waiting for the char.
 *  If wait_time == n  we wait for n msec for a character to arrive.
 *  If wait_time == -1 we wait forever for a character to arrive.
 *
 *  Return the number of obtained characters.
 *  Return -1 when end of input script reached.
 */
    int
inchar(
    char_u	*buf,
    int		maxlen,
    long	wait_time,	    /* milli seconds */
    int		tb_change_cnt)
{
    int		len = 0;	    /* init for GCC */
    int		retesc = FALSE;	    /* return ESC with gotint */
    int		script_char;

    if (wait_time == -1L || wait_time > 100L)  /* flush output before waiting */
    {
	cursor_on();
	out_flush();
#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    gui_update_cursor(FALSE, FALSE);
# ifdef FEAT_MOUSESHAPE
	    if (postponed_mouseshape)
		update_mouseshape(-1);
# endif
	}
#endif
    }

    /*
     * Don't reset these when at the hit-return prompt, otherwise a endless
     * recursive loop may result (write error in swapfile, hit-return, timeout
     * on char wait, flush swapfile, write error....).
     */
    if (State != HITRETURN)
    {
	did_outofmem_msg = FALSE;   /* display out of memory message (again) */
	did_swapwrite_msg = FALSE;  /* display swap file write error again */
    }
    undo_off = FALSE;		    /* restart undo now */

    /*
     * Get a character from a script file if there is one.
     * If interrupted: Stop reading script files, close them all.
     */
    script_char = -1;
    while (scriptin[curscript] != NULL && script_char < 0
#ifdef FEAT_EVAL
	    && !ignore_script
#endif
	    )
    {

#ifdef MESSAGE_QUEUE
	parse_queued_messages();
#endif

	if (got_int || (script_char = getc(scriptin[curscript])) < 0)
	{
	    /* Reached EOF.
	     * Careful: closescript() frees typebuf.tb_buf[] and buf[] may
	     * point inside typebuf.tb_buf[].  Don't use buf[] after this! */
	    closescript();
	    /*
	     * When reading script file is interrupted, return an ESC to get
	     * back to normal mode.
	     * Otherwise return -1, because typebuf.tb_buf[] has changed.
	     */
	    if (got_int)
		retesc = TRUE;
	    else
		return -1;
	}
	else
	{
	    buf[0] = script_char;
	    len = 1;
	}
    }

    if (script_char < 0)	/* did not get a character from script */
    {
	/*
	 * If we got an interrupt, skip all previously typed characters and
	 * return TRUE if quit reading script file.
	 * Stop reading typeahead when a single CTRL-C was read,
	 * fill_input_buf() returns this when not able to read from stdin.
	 * Don't use buf[] here, closescript() may have freed typebuf.tb_buf[]
	 * and buf may be pointing inside typebuf.tb_buf[].
	 */
	if (got_int)
	{
#define DUM_LEN MAXMAPLEN * 3 + 3
	    char_u	dum[DUM_LEN + 1];

	    for (;;)
	    {
		len = ui_inchar(dum, DUM_LEN, 0L, 0);
		if (len == 0 || (len == 1 && dum[0] == 3))
		    break;
	    }
	    return retesc;
	}

	/*
	 * Always flush the output characters when getting input characters
	 * from the user.
	 */
	out_flush();

	/*
	 * Fill up to a third of the buffer, because each character may be
	 * tripled below.
	 */
	len = ui_inchar(buf, maxlen / 3, wait_time, tb_change_cnt);
    }

    if (typebuf_changed(tb_change_cnt))
	return 0;

    return fix_input_buffer(buf, len, script_char >= 0);
}

/*
 * Fix typed characters for use by vgetc() and check_termcode().
 * buf[] must have room to triple the number of bytes!
 * Returns the new length.
 */
    int
fix_input_buffer(
    char_u	*buf,
    int		len,
    int		script)		/* TRUE when reading from a script */
{
    int		i;
    char_u	*p = buf;

    /*
     * Two characters are special: NUL and K_SPECIAL.
     * When compiled With the GUI CSI is also special.
     * Replace	     NUL by K_SPECIAL KS_ZERO	 KE_FILLER
     * Replace K_SPECIAL by K_SPECIAL KS_SPECIAL KE_FILLER
     * Replace       CSI by K_SPECIAL KS_EXTRA   KE_CSI
     * Don't replace K_SPECIAL when reading a script file.
     */
    for (i = len; --i >= 0; ++p)
    {
#ifdef FEAT_GUI
	/* When the GUI is used any character can come after a CSI, don't
	 * escape it. */
	if (gui.in_use && p[0] == CSI && i >= 2)
	{
	    p += 2;
	    i -= 2;
	}
	/* When the GUI is not used CSI needs to be escaped. */
	else if (!gui.in_use && p[0] == CSI)
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    *p++ = K_SPECIAL;
	    *p++ = KS_EXTRA;
	    *p = (int)KE_CSI;
	    len += 2;
	}
	else
#endif
	if (p[0] == NUL || (p[0] == K_SPECIAL && !script
#ifdef FEAT_AUTOCMD
		    /* timeout may generate K_CURSORHOLD */
		    && (i < 2 || p[1] != KS_EXTRA || p[2] != (int)KE_CURSORHOLD)
#endif
#if defined(WIN3264) && !defined(FEAT_GUI)
		    /* Win32 console passes modifiers */
		    && (i < 2 || p[1] != KS_MODIFIER)
#endif
		    ))
	{
	    mch_memmove(p + 3, p + 1, (size_t)i);
	    p[2] = K_THIRD(p[0]);
	    p[1] = K_SECOND(p[0]);
	    p[0] = K_SPECIAL;
	    p += 2;
	    len += 2;
	}
    }
    *p = NUL;		/* add trailing NUL */
    return len;
}

#if defined(USE_INPUT_BUF) || defined(PROTO)
/*
 * Return TRUE when bytes are in the input buffer or in the typeahead buffer.
 * Normally the input buffer would be sufficient, but the server_to_input_buf()
 * or feedkeys() may insert characters in the typeahead buffer while we are
 * waiting for input to arrive.
 */
    int
input_available(void)
{
    return (!vim_is_input_buf_empty()
# if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
# endif
	    );
}
#endif

/*
 * map[!]		    : show all key mappings
 * map[!] {lhs}		    : show key mapping for {lhs}
 * map[!] {lhs} {rhs}	    : set key mapping for {lhs} to {rhs}
 * noremap[!] {lhs} {rhs}   : same, but no remapping for {rhs}
 * unmap[!] {lhs}	    : remove key mapping for {lhs}
 * abbr			    : show all abbreviations
 * abbr {lhs}		    : show abbreviations for {lhs}
 * abbr {lhs} {rhs}	    : set abbreviation for {lhs} to {rhs}
 * noreabbr {lhs} {rhs}	    : same, but no remapping for {rhs}
 * unabbr {lhs}		    : remove abbreviation for {lhs}
 *
 * maptype: 0 for :map, 1 for :unmap, 2 for noremap.
 *
 * arg is pointer to any arguments. Note: arg cannot be a read-only string,
 * it will be modified.
 *
 * for :map   mode is NORMAL + VISUAL + SELECTMODE + OP_PENDING
 * for :map!  mode is INSERT + CMDLINE
 * for :cmap  mode is CMDLINE
 * for :imap  mode is INSERT
 * for :lmap  mode is LANGMAP
 * for :nmap  mode is NORMAL
 * for :vmap  mode is VISUAL + SELECTMODE
 * for :xmap  mode is VISUAL
 * for :smap  mode is SELECTMODE
 * for :omap  mode is OP_PENDING
 *
 * for :abbr  mode is INSERT + CMDLINE
 * for :iabbr mode is INSERT
 * for :cabbr mode is CMDLINE
 *
 * Return 0 for success
 *	  1 for invalid arguments
 *	  2 for no match
 *	  4 for out of mem
 *	  5 for entry not unique
 */
    int
do_map(
    int		maptype,
    char_u	*arg,
    int		mode,
    int		abbrev)		/* not a mapping but an abbreviation */
{
    char_u	*keys;
    mapblock_T	*mp, **mpp;
    char_u	*rhs;
    char_u	*p;
    int		n;
    int		len = 0;	/* init for GCC */
    char_u	*newstr;
    int		hasarg;
    int		haskey;
    int		did_it = FALSE;
#ifdef FEAT_LOCALMAP
    int		did_local = FALSE;
#endif
    int		round;
    char_u	*keys_buf = NULL;
    char_u	*arg_buf = NULL;
    int		retval = 0;
    int		do_backslash;
    int		hash;
    int		new_hash;
    mapblock_T	**abbr_table;
    mapblock_T	**map_table;
    int		unique = FALSE;
    int		nowait = FALSE;
    int		silent = FALSE;
    int		special = FALSE;
#ifdef FEAT_EVAL
    int		expr = FALSE;
#endif
    int		noremap;
    char_u      *orig_rhs;

    keys = arg;
    map_table = maphash;
    abbr_table = &first_abbr;

    /* For ":noremap" don't remap, otherwise do remap. */
    if (maptype == 2)
	noremap = REMAP_NONE;
    else
	noremap = REMAP_YES;

    /* Accept <buffer>, <nowait>, <silent>, <expr> <script> and <unique> in
     * any order. */
    for (;;)
    {
#ifdef FEAT_LOCALMAP
	/*
	 * Check for "<buffer>": mapping local to buffer.
	 */
	if (STRNCMP(keys, "<buffer>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    map_table = curbuf->b_maphash;
	    abbr_table = &curbuf->b_first_abbr;
	    continue;
	}
#endif

	/*
	 * Check for "<nowait>": don't wait for more characters.
	 */
	if (STRNCMP(keys, "<nowait>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    nowait = TRUE;
	    continue;
	}

	/*
	 * Check for "<silent>": don't echo commands.
	 */
	if (STRNCMP(keys, "<silent>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    silent = TRUE;
	    continue;
	}

	/*
	 * Check for "<special>": accept special keys in <>
	 */
	if (STRNCMP(keys, "<special>", 9) == 0)
	{
	    keys = skipwhite(keys + 9);
	    special = TRUE;
	    continue;
	}

#ifdef FEAT_EVAL
	/*
	 * Check for "<script>": remap script-local mappings only
	 */
	if (STRNCMP(keys, "<script>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    noremap = REMAP_SCRIPT;
	    continue;
	}

	/*
	 * Check for "<expr>": {rhs} is an expression.
	 */
	if (STRNCMP(keys, "<expr>", 6) == 0)
	{
	    keys = skipwhite(keys + 6);
	    expr = TRUE;
	    continue;
	}
#endif
	/*
	 * Check for "<unique>": don't overwrite an existing mapping.
	 */
	if (STRNCMP(keys, "<unique>", 8) == 0)
	{
	    keys = skipwhite(keys + 8);
	    unique = TRUE;
	    continue;
	}
	break;
    }

    validate_maphash();

    /*
     * Find end of keys and skip CTRL-Vs (and backslashes) in it.
     * Accept backslash like CTRL-V when 'cpoptions' does not contain 'B'.
     * with :unmap white space is included in the keys, no argument possible.
     */
    p = keys;
    do_backslash = (vim_strchr(p_cpo, CPO_BSLASH) == NULL);
    while (*p && (maptype == 1 || !vim_iswhite(*p)))
    {
	if ((p[0] == Ctrl_V || (do_backslash && p[0] == '\\')) &&
								  p[1] != NUL)
	    ++p;		/* skip CTRL-V or backslash */
	++p;
    }
    if (*p != NUL)
	*p++ = NUL;

    p = skipwhite(p);
    rhs = p;
    hasarg = (*rhs != NUL);
    haskey = (*keys != NUL);

    /* check for :unmap without argument */
    if (maptype == 1 && !haskey)
    {
	retval = 1;
	goto theend;
    }

    /*
     * If mapping has been given as ^V<C_UP> say, then replace the term codes
     * with the appropriate two bytes. If it is a shifted special key, unshift
     * it too, giving another two bytes.
     * replace_termcodes() may move the result to allocated memory, which
     * needs to be freed later (*keys_buf and *arg_buf).
     * replace_termcodes() also removes CTRL-Vs and sometimes backslashes.
     */
    if (haskey)
	keys = replace_termcodes(keys, &keys_buf, TRUE, TRUE, special);
    orig_rhs = rhs;
    if (hasarg)
    {
	if (STRICMP(rhs, "<nop>") == 0)	    /* "<Nop>" means nothing */
	    rhs = (char_u *)"";
	else
	    rhs = replace_termcodes(rhs, &arg_buf, FALSE, TRUE, special);
    }

#ifdef FEAT_FKMAP
    /*
     * When in right-to-left mode and alternate keymap option set,
     * reverse the character flow in the rhs in Farsi.
     */
    if (p_altkeymap && curwin->w_p_rl)
	lrswap(rhs);
#endif

    /*
     * check arguments and translate function keys
     */
    if (haskey)
    {
	len = (int)STRLEN(keys);
	if (len > MAXMAPLEN)		/* maximum length of MAXMAPLEN chars */
	{
	    retval = 1;
	    goto theend;
	}

	if (abbrev && maptype != 1)
	{
	    /*
	     * If an abbreviation ends in a keyword character, the
	     * rest must be all keyword-char or all non-keyword-char.
	     * Otherwise we won't be able to find the start of it in a
	     * vi-compatible way.
	     */
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		int	first, last;
		int	same = -1;

		first = vim_iswordp(keys);
		last = first;
		p = keys + (*mb_ptr2len)(keys);
		n = 1;
		while (p < keys + len)
		{
		    ++n;			/* nr of (multi-byte) chars */
		    last = vim_iswordp(p);	/* type of last char */
		    if (same == -1 && last != first)
			same = n - 1;		/* count of same char type */
		    p += (*mb_ptr2len)(p);
		}
		if (last && n > 2 && same >= 0 && same < n - 1)
		{
		    retval = 1;
		    goto theend;
		}
	    }
	    else
#endif
		if (vim_iswordc(keys[len - 1]))	/* ends in keyword char */
		    for (n = 0; n < len - 2; ++n)
			if (vim_iswordc(keys[n]) != vim_iswordc(keys[len - 2]))
			{
			    retval = 1;
			    goto theend;
			}
	    /* An abbreviation cannot contain white space. */
	    for (n = 0; n < len; ++n)
		if (vim_iswhite(keys[n]))
		{
		    retval = 1;
		    goto theend;
		}
	}
    }

    if (haskey && hasarg && abbrev)	/* if we will add an abbreviation */
	no_abbr = FALSE;		/* reset flag that indicates there are
							    no abbreviations */

    if (!haskey || (maptype != 1 && !hasarg))
	msg_start();

#ifdef FEAT_LOCALMAP
    /*
     * Check if a new local mapping wasn't already defined globally.
     */
    if (map_table == curbuf->b_maphash && haskey && hasarg && maptype != 1)
    {
	/* need to loop over all global hash lists */
	for (hash = 0; hash < 256 && !got_int; ++hash)
	{
	    if (abbrev)
	    {
		if (hash != 0)	/* there is only one abbreviation list */
		    break;
		mp = first_abbr;
	    }
	    else
		mp = maphash[hash];
	    for ( ; mp != NULL && !got_int; mp = mp->m_next)
	    {
		/* check entries with the same mode */
		if ((mp->m_mode & mode) != 0
			&& mp->m_keylen == len
			&& unique
			&& STRNCMP(mp->m_keys, keys, (size_t)len) == 0)
		{
		    if (abbrev)
			EMSG2(_("E224: global abbreviation already exists for %s"),
				mp->m_keys);
		    else
			EMSG2(_("E225: global mapping already exists for %s"),
				mp->m_keys);
		    retval = 5;
		    goto theend;
		}
	    }
	}
    }

    /*
     * When listing global mappings, also list buffer-local ones here.
     */
    if (map_table != curbuf->b_maphash && !hasarg && maptype != 1)
    {
	/* need to loop over all global hash lists */
	for (hash = 0; hash < 256 && !got_int; ++hash)
	{
	    if (abbrev)
	    {
		if (hash != 0)	/* there is only one abbreviation list */
		    break;
		mp = curbuf->b_first_abbr;
	    }
	    else
		mp = curbuf->b_maphash[hash];
	    for ( ; mp != NULL && !got_int; mp = mp->m_next)
	    {
		/* check entries with the same mode */
		if ((mp->m_mode & mode) != 0)
		{
		    if (!haskey)		    /* show all entries */
		    {
			showmap(mp, TRUE);
			did_local = TRUE;
		    }
		    else
		    {
			n = mp->m_keylen;
			if (STRNCMP(mp->m_keys, keys,
					    (size_t)(n < len ? n : len)) == 0)
			{
			    showmap(mp, TRUE);
			    did_local = TRUE;
			}
		    }
		}
	    }
	}
    }
#endif

    /*
     * Find an entry in the maphash[] list that matches.
     * For :unmap we may loop two times: once to try to unmap an entry with a
     * matching 'from' part, a second time, if the first fails, to unmap an
     * entry with a matching 'to' part. This was done to allow ":ab foo bar"
     * to be unmapped by typing ":unab foo", where "foo" will be replaced by
     * "bar" because of the abbreviation.
     */
    for (round = 0; (round == 0 || maptype == 1) && round <= 1
					      && !did_it && !got_int; ++round)
    {
	/* need to loop over all hash lists */
	for (hash = 0; hash < 256 && !got_int; ++hash)
	{
	    if (abbrev)
	    {
		if (hash > 0)	/* there is only one abbreviation list */
		    break;
		mpp = abbr_table;
	    }
	    else
		mpp = &(map_table[hash]);
	    for (mp = *mpp; mp != NULL && !got_int; mp = *mpp)
	    {

		if (!(mp->m_mode & mode))   /* skip entries with wrong mode */
		{
		    mpp = &(mp->m_next);
		    continue;
		}
		if (!haskey)		    /* show all entries */
		{
		    showmap(mp, map_table != maphash);
		    did_it = TRUE;
		}
		else			    /* do we have a match? */
		{
		    if (round)	    /* second round: Try unmap "rhs" string */
		    {
			n = (int)STRLEN(mp->m_str);
			p = mp->m_str;
		    }
		    else
		    {
			n = mp->m_keylen;
			p = mp->m_keys;
		    }
		    if (STRNCMP(p, keys, (size_t)(n < len ? n : len)) == 0)
		    {
			if (maptype == 1)	/* delete entry */
			{
			    /* Only accept a full match.  For abbreviations we
			     * ignore trailing space when matching with the
			     * "lhs", since an abbreviation can't have
			     * trailing space. */
			    if (n != len && (!abbrev || round || n > len
					       || *skipwhite(keys + n) != NUL))
			    {
				mpp = &(mp->m_next);
				continue;
			    }
			    /*
			     * We reset the indicated mode bits. If nothing is
			     * left the entry is deleted below.
			     */
			    mp->m_mode &= ~mode;
			    did_it = TRUE;	/* remember we did something */
			}
			else if (!hasarg)	/* show matching entry */
			{
			    showmap(mp, map_table != maphash);
			    did_it = TRUE;
			}
			else if (n != len)	/* new entry is ambiguous */
			{
			    mpp = &(mp->m_next);
			    continue;
			}
			else if (unique)
			{
			    if (abbrev)
				EMSG2(_("E226: abbreviation already exists for %s"),
									   p);
			    else
				EMSG2(_("E227: mapping already exists for %s"), p);
			    retval = 5;
			    goto theend;
			}
			else			/* new rhs for existing entry */
			{
			    mp->m_mode &= ~mode;	/* remove mode bits */
			    if (mp->m_mode == 0 && !did_it) /* reuse entry */
			    {
				newstr = vim_strsave(rhs);
				if (newstr == NULL)
				{
				    retval = 4;		/* no mem */
				    goto theend;
				}
				vim_free(mp->m_str);
				mp->m_str = newstr;
				vim_free(mp->m_orig_str);
				mp->m_orig_str = vim_strsave(orig_rhs);
				mp->m_noremap = noremap;
				mp->m_nowait = nowait;
				mp->m_silent = silent;
				mp->m_mode = mode;
#ifdef FEAT_EVAL
				mp->m_expr = expr;
				mp->m_script_ID = current_SID;
#endif
				did_it = TRUE;
			    }
			}
			if (mp->m_mode == 0)	/* entry can be deleted */
			{
			    map_free(mpp);
			    continue;		/* continue with *mpp */
			}

			/*
			 * May need to put this entry into another hash list.
			 */
			new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
			if (!abbrev && new_hash != hash)
			{
			    *mpp = mp->m_next;
			    mp->m_next = map_table[new_hash];
			    map_table[new_hash] = mp;

			    continue;		/* continue with *mpp */
			}
		    }
		}
		mpp = &(mp->m_next);
	    }
	}
    }

    if (maptype == 1)			    /* delete entry */
    {
	if (!did_it)
	    retval = 2;			    /* no match */
	else if (*keys == Ctrl_C)
	{
	    /* If CTRL-C has been unmapped, reuse it for Interrupting. */
#ifdef FEAT_LOCALMAP
	    if (map_table == curbuf->b_maphash)
		curbuf->b_mapped_ctrl_c &= ~mode;
	    else
#endif
		mapped_ctrl_c &= ~mode;
	}
	goto theend;
    }

    if (!haskey || !hasarg)		    /* print entries */
    {
	if (!did_it
#ifdef FEAT_LOCALMAP
		&& !did_local
#endif
		)
	{
	    if (abbrev)
		MSG(_("No abbreviation found"));
	    else
		MSG(_("No mapping found"));
	}
	goto theend;			    /* listing finished */
    }

    if (did_it)			/* have added the new entry already */
	goto theend;

    /*
     * Get here when adding a new entry to the maphash[] list or abbrlist.
     */
    mp = (mapblock_T *)alloc((unsigned)sizeof(mapblock_T));
    if (mp == NULL)
    {
	retval = 4;	    /* no mem */
	goto theend;
    }

    /* If CTRL-C has been mapped, don't always use it for Interrupting. */
    if (*keys == Ctrl_C)
    {
#ifdef FEAT_LOCALMAP
	if (map_table == curbuf->b_maphash)
	    curbuf->b_mapped_ctrl_c |= mode;
	else
#endif
	    mapped_ctrl_c |= mode;
    }

    mp->m_keys = vim_strsave(keys);
    mp->m_str = vim_strsave(rhs);
    mp->m_orig_str = vim_strsave(orig_rhs);
    if (mp->m_keys == NULL || mp->m_str == NULL)
    {
	vim_free(mp->m_keys);
	vim_free(mp->m_str);
	vim_free(mp->m_orig_str);
	vim_free(mp);
	retval = 4;	/* no mem */
	goto theend;
    }
    mp->m_keylen = (int)STRLEN(mp->m_keys);
    mp->m_noremap = noremap;
    mp->m_nowait = nowait;
    mp->m_silent = silent;
    mp->m_mode = mode;
#ifdef FEAT_EVAL
    mp->m_expr = expr;
    mp->m_script_ID = current_SID;
#endif

    /* add the new entry in front of the abbrlist or maphash[] list */
    if (abbrev)
    {
	mp->m_next = *abbr_table;
	*abbr_table = mp;
    }
    else
    {
	n = MAP_HASH(mp->m_mode, mp->m_keys[0]);
	mp->m_next = map_table[n];
	map_table[n] = mp;
    }

theend:
    vim_free(keys_buf);
    vim_free(arg_buf);
    return retval;
}

/*
 * Delete one entry from the abbrlist or maphash[].
 * "mpp" is a pointer to the m_next field of the PREVIOUS entry!
 */
    static void
map_free(mapblock_T **mpp)
{
    mapblock_T	*mp;

    mp = *mpp;
    vim_free(mp->m_keys);
    vim_free(mp->m_str);
    vim_free(mp->m_orig_str);
    *mpp = mp->m_next;
    vim_free(mp);
}

/*
 * Initialize maphash[] for first use.
 */
    static void
validate_maphash(void)
{
    if (!maphash_valid)
    {
	vim_memset(maphash, 0, sizeof(maphash));
	maphash_valid = TRUE;
    }
}

/*
 * Get the mapping mode from the command name.
 */
    int
get_map_mode(char_u **cmdp, int forceit)
{
    char_u	*p;
    int		modec;
    int		mode;

    p = *cmdp;
    modec = *p++;
    if (modec == 'i')
	mode = INSERT;				/* :imap */
    else if (modec == 'l')
	mode = LANGMAP;				/* :lmap */
    else if (modec == 'c')
	mode = CMDLINE;				/* :cmap */
    else if (modec == 'n' && *p != 'o')		    /* avoid :noremap */
	mode = NORMAL;				/* :nmap */
    else if (modec == 'v')
	mode = VISUAL + SELECTMODE;		/* :vmap */
    else if (modec == 'x')
	mode = VISUAL;				/* :xmap */
    else if (modec == 's')
	mode = SELECTMODE;			/* :smap */
    else if (modec == 'o')
	mode = OP_PENDING;			/* :omap */
    else
    {
	--p;
	if (forceit)
	    mode = INSERT + CMDLINE;		/* :map ! */
	else
	    mode = VISUAL + SELECTMODE + NORMAL + OP_PENDING;/* :map */
    }

    *cmdp = p;
    return mode;
}

/*
 * Clear all mappings or abbreviations.
 * 'abbr' should be FALSE for mappings, TRUE for abbreviations.
 */
    void
map_clear(
    char_u	*cmdp,
    char_u	*arg UNUSED,
    int		forceit,
    int		abbr)
{
    int		mode;
#ifdef FEAT_LOCALMAP
    int		local;

    local = (STRCMP(arg, "<buffer>") == 0);
    if (!local && *arg != NUL)
    {
	EMSG(_(e_invarg));
	return;
    }
#endif

    mode = get_map_mode(&cmdp, forceit);
    map_clear_int(curbuf, mode,
#ifdef FEAT_LOCALMAP
	    local,
#else
	    FALSE,
#endif
	    abbr);
}

/*
 * Clear all mappings in "mode".
 */
    void
map_clear_int(
    buf_T	*buf UNUSED,	/* buffer for local mappings */
    int		mode,		/* mode in which to delete */
    int		local UNUSED,	/* TRUE for buffer-local mappings */
    int		abbr)		/* TRUE for abbreviations */
{
    mapblock_T	*mp, **mpp;
    int		hash;
    int		new_hash;

    validate_maphash();

    for (hash = 0; hash < 256; ++hash)
    {
	if (abbr)
	{
	    if (hash > 0)	/* there is only one abbrlist */
		break;
#ifdef FEAT_LOCALMAP
	    if (local)
		mpp = &buf->b_first_abbr;
	    else
#endif
		mpp = &first_abbr;
	}
	else
	{
#ifdef FEAT_LOCALMAP
	    if (local)
		mpp = &buf->b_maphash[hash];
	    else
#endif
		mpp = &maphash[hash];
	}
	while (*mpp != NULL)
	{
	    mp = *mpp;
	    if (mp->m_mode & mode)
	    {
		mp->m_mode &= ~mode;
		if (mp->m_mode == 0) /* entry can be deleted */
		{
		    map_free(mpp);
		    continue;
		}
		/*
		 * May need to put this entry into another hash list.
		 */
		new_hash = MAP_HASH(mp->m_mode, mp->m_keys[0]);
		if (!abbr && new_hash != hash)
		{
		    *mpp = mp->m_next;
#ifdef FEAT_LOCALMAP
		    if (local)
		    {
			mp->m_next = buf->b_maphash[new_hash];
			buf->b_maphash[new_hash] = mp;
		    }
		    else
#endif
		    {
			mp->m_next = maphash[new_hash];
			maphash[new_hash] = mp;
		    }
		    continue;		/* continue with *mpp */
		}
	    }
	    mpp = &(mp->m_next);
	}
    }
}

/*
 * Return characters to represent the map mode in an allocated string.
 * Returns NULL when out of memory.
 */
    char_u *
map_mode_to_chars(int mode)
{
    garray_T    mapmode;

    ga_init2(&mapmode, 1, 7);

    if ((mode & (INSERT + CMDLINE)) == INSERT + CMDLINE)
	ga_append(&mapmode, '!');			/* :map! */
    else if (mode & INSERT)
	ga_append(&mapmode, 'i');			/* :imap */
    else if (mode & LANGMAP)
	ga_append(&mapmode, 'l');			/* :lmap */
    else if (mode & CMDLINE)
	ga_append(&mapmode, 'c');			/* :cmap */
    else if ((mode & (NORMAL + VISUAL + SELECTMODE + OP_PENDING))
				 == NORMAL + VISUAL + SELECTMODE + OP_PENDING)
	ga_append(&mapmode, ' ');			/* :map */
    else
    {
	if (mode & NORMAL)
	    ga_append(&mapmode, 'n');			/* :nmap */
	if (mode & OP_PENDING)
	    ga_append(&mapmode, 'o');			/* :omap */
	if ((mode & (VISUAL + SELECTMODE)) == VISUAL + SELECTMODE)
	    ga_append(&mapmode, 'v');			/* :vmap */
	else
	{
	    if (mode & VISUAL)
		ga_append(&mapmode, 'x');		/* :xmap */
	    if (mode & SELECTMODE)
		ga_append(&mapmode, 's');		/* :smap */
	}
    }

    ga_append(&mapmode, NUL);
    return (char_u *)mapmode.ga_data;
}

    static void
showmap(
    mapblock_T	*mp,
    int		local)	    /* TRUE for buffer-local map */
{
    int		len = 1;
    char_u	*mapchars;

    if (msg_didout || msg_silent != 0)
    {
	msg_putchar('\n');
	if (got_int)	    /* 'q' typed at MORE prompt */
	    return;
    }

    mapchars = map_mode_to_chars(mp->m_mode);
    if (mapchars != NULL)
    {
	msg_puts(mapchars);
	len = (int)STRLEN(mapchars);
	vim_free(mapchars);
    }

    while (++len <= 3)
	msg_putchar(' ');

    /* Display the LHS.  Get length of what we write. */
    len = msg_outtrans_special(mp->m_keys, TRUE);
    do
    {
	msg_putchar(' ');		/* padd with blanks */
	++len;
    } while (len < 12);

    if (mp->m_noremap == REMAP_NONE)
	msg_puts_attr((char_u *)"*", hl_attr(HLF_8));
    else if (mp->m_noremap == REMAP_SCRIPT)
	msg_puts_attr((char_u *)"&", hl_attr(HLF_8));
    else
	msg_putchar(' ');

    if (local)
	msg_putchar('@');
    else
	msg_putchar(' ');

    /* Use FALSE below if we only want things like <Up> to show up as such on
     * the rhs, and not M-x etc, TRUE gets both -- webb */
    if (*mp->m_str == NUL)
	msg_puts_attr((char_u *)"<Nop>", hl_attr(HLF_8));
    else
    {
	/* Remove escaping of CSI, because "m_str" is in a format to be used
	 * as typeahead. */
	char_u *s = vim_strsave(mp->m_str);
	if (s != NULL)
	{
	    vim_unescape_csi(s);
	    msg_outtrans_special(s, FALSE);
	    vim_free(s);
	}
    }
#ifdef FEAT_EVAL
    if (p_verbose > 0)
	last_set_msg(mp->m_script_ID);
#endif
    out_flush();			/* show one line at a time */
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "modechars".
 * Recognize termcap codes in "str".
 * Also checks mappings local to the current buffer.
 */
    int
map_to_exists(char_u *str, char_u *modechars, int abbr)
{
    int		mode = 0;
    char_u	*rhs;
    char_u	*buf;
    int		retval;

    rhs = replace_termcodes(str, &buf, FALSE, TRUE, FALSE);

    if (vim_strchr(modechars, 'n') != NULL)
	mode |= NORMAL;
    if (vim_strchr(modechars, 'v') != NULL)
	mode |= VISUAL + SELECTMODE;
    if (vim_strchr(modechars, 'x') != NULL)
	mode |= VISUAL;
    if (vim_strchr(modechars, 's') != NULL)
	mode |= SELECTMODE;
    if (vim_strchr(modechars, 'o') != NULL)
	mode |= OP_PENDING;
    if (vim_strchr(modechars, 'i') != NULL)
	mode |= INSERT;
    if (vim_strchr(modechars, 'l') != NULL)
	mode |= LANGMAP;
    if (vim_strchr(modechars, 'c') != NULL)
	mode |= CMDLINE;

    retval = map_to_exists_mode(rhs, mode, abbr);
    vim_free(buf);

    return retval;
}
#endif

/*
 * Return TRUE if a map exists that has "str" in the rhs for mode "mode".
 * Also checks mappings local to the current buffer.
 */
    int
map_to_exists_mode(char_u *rhs, int mode, int abbr)
{
    mapblock_T	*mp;
    int		hash;
# ifdef FEAT_LOCALMAP
    int		expand_buffer = FALSE;

    validate_maphash();

    /* Do it twice: once for global maps and once for local maps. */
    for (;;)
    {
# endif
	for (hash = 0; hash < 256; ++hash)
	{
	    if (abbr)
	    {
		if (hash > 0)		/* there is only one abbr list */
		    break;
#ifdef FEAT_LOCALMAP
		if (expand_buffer)
		    mp = curbuf->b_first_abbr;
		else
#endif
		    mp = first_abbr;
	    }
# ifdef FEAT_LOCALMAP
	    else if (expand_buffer)
		mp = curbuf->b_maphash[hash];
# endif
	    else
		mp = maphash[hash];
	    for (; mp; mp = mp->m_next)
	    {
		if ((mp->m_mode & mode)
			&& strstr((char *)mp->m_str, (char *)rhs) != NULL)
		    return TRUE;
	    }
	}
# ifdef FEAT_LOCALMAP
	if (expand_buffer)
	    break;
	expand_buffer = TRUE;
    }
# endif

    return FALSE;
}

#if defined(FEAT_CMDL_COMPL) || defined(PROTO)
/*
 * Used below when expanding mapping/abbreviation names.
 */
static int	expand_mapmodes = 0;
static int	expand_isabbrev = 0;
#ifdef FEAT_LOCALMAP
static int	expand_buffer = FALSE;
#endif

/*
 * Work out what to complete when doing command line completion of mapping
 * or abbreviation names.
 */
    char_u *
set_context_in_map_cmd(
    expand_T	*xp,
    char_u	*cmd,
    char_u	*arg,
    int		forceit,	/* TRUE if '!' given */
    int		isabbrev,	/* TRUE if abbreviation */
    int		isunmap,	/* TRUE if unmap/unabbrev command */
    cmdidx_T	cmdidx)
{
    if (forceit && cmdidx != CMD_map && cmdidx != CMD_unmap)
	xp->xp_context = EXPAND_NOTHING;
    else
    {
	if (isunmap)
	    expand_mapmodes = get_map_mode(&cmd, forceit || isabbrev);
	else
	{
	    expand_mapmodes = INSERT + CMDLINE;
	    if (!isabbrev)
		expand_mapmodes += VISUAL + SELECTMODE + NORMAL + OP_PENDING;
	}
	expand_isabbrev = isabbrev;
	xp->xp_context = EXPAND_MAPPINGS;
#ifdef FEAT_LOCALMAP
	expand_buffer = FALSE;
#endif
	for (;;)
	{
#ifdef FEAT_LOCALMAP
	    if (STRNCMP(arg, "<buffer>", 8) == 0)
	    {
		expand_buffer = TRUE;
		arg = skipwhite(arg + 8);
		continue;
	    }
#endif
	    if (STRNCMP(arg, "<unique>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<nowait>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<silent>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
#ifdef FEAT_EVAL
	    if (STRNCMP(arg, "<script>", 8) == 0)
	    {
		arg = skipwhite(arg + 8);
		continue;
	    }
	    if (STRNCMP(arg, "<expr>", 6) == 0)
	    {
		arg = skipwhite(arg + 6);
		continue;
	    }
#endif
	    break;
	}
	xp->xp_pattern = arg;
    }

    return NULL;
}

/*
 * Find all mapping/abbreviation names that match regexp 'prog'.
 * For command line expansion of ":[un]map" and ":[un]abbrev" in all modes.
 * Return OK if matches found, FAIL otherwise.
 */
    int
ExpandMappings(
    regmatch_T	*regmatch,
    int		*num_file,
    char_u	***file)
{
    mapblock_T	*mp;
    int		hash;
    int		count;
    int		round;
    char_u	*p;
    int		i;

    validate_maphash();

    *num_file = 0;		    /* return values in case of FAIL */
    *file = NULL;

    /*
     * round == 1: Count the matches.
     * round == 2: Build the array to keep the matches.
     */
    for (round = 1; round <= 2; ++round)
    {
	count = 0;

	for (i = 0; i < 6; ++i)
	{
	    if (i == 0)
		p = (char_u *)"<silent>";
	    else if (i == 1)
		p = (char_u *)"<unique>";
#ifdef FEAT_EVAL
	    else if (i == 2)
		p = (char_u *)"<script>";
	    else if (i == 3)
		p = (char_u *)"<expr>";
#endif
#ifdef FEAT_LOCALMAP
	    else if (i == 4 && !expand_buffer)
		p = (char_u *)"<buffer>";
#endif
	    else if (i == 5)
		p = (char_u *)"<nowait>";
	    else
		continue;

	    if (vim_regexec(regmatch, p, (colnr_T)0))
	    {
		if (round == 1)
		    ++count;
		else
		    (*file)[count++] = vim_strsave(p);
	    }
	}

	for (hash = 0; hash < 256; ++hash)
	{
	    if (expand_isabbrev)
	    {
		if (hash > 0)	/* only one abbrev list */
		    break; /* for (hash) */
		mp = first_abbr;
	    }
#ifdef FEAT_LOCALMAP
	    else if (expand_buffer)
		mp = curbuf->b_maphash[hash];
#endif
	    else
		mp = maphash[hash];
	    for (; mp; mp = mp->m_next)
	    {
		if (mp->m_mode & expand_mapmodes)
		{
		    p = translate_mapping(mp->m_keys, TRUE);
		    if (p != NULL && vim_regexec(regmatch, p, (colnr_T)0))
		    {
			if (round == 1)
			    ++count;
			else
			{
			    (*file)[count++] = p;
			    p = NULL;
			}
		    }
		    vim_free(p);
		}
	    } /* for (mp) */
	} /* for (hash) */

	if (count == 0)			/* no match found */
	    break; /* for (round) */

	if (round == 1)
	{
	    *file = (char_u **)alloc((unsigned)(count * sizeof(char_u *)));
	    if (*file == NULL)
		return FAIL;
	}
    } /* for (round) */

    if (count > 1)
    {
	char_u	**ptr1;
	char_u	**ptr2;
	char_u	**ptr3;

	/* Sort the matches */
	sort_strings(*file, count);

	/* Remove multiple entries */
	ptr1 = *file;
	ptr2 = ptr1 + 1;
	ptr3 = ptr1 + count;

	while (ptr2 < ptr3)
	{
	    if (STRCMP(*ptr1, *ptr2))
		*++ptr1 = *ptr2++;
	    else
	    {
		vim_free(*ptr2++);
		count--;
	    }
	}
    }

    *num_file = count;
    return (count == 0 ? FAIL : OK);
}
#endif /* FEAT_CMDL_COMPL */

/*
 * Check for an abbreviation.
 * Cursor is at ptr[col]. When inserting, mincol is where insert started.
 * "c" is the character typed before check_abbr was called.  It may have
 * ABBR_OFF added to avoid prepending a CTRL-V to it.
 *
 * Historic vi practice: The last character of an abbreviation must be an id
 * character ([a-zA-Z0-9_]). The characters in front of it must be all id
 * characters or all non-id characters. This allows for abbr. "#i" to
 * "#include".
 *
 * Vim addition: Allow for abbreviations that end in a non-keyword character.
 * Then there must be white space before the abbr.
 *
 * return TRUE if there is an abbreviation, FALSE if not
 */
    int
check_abbr(
    int		c,
    char_u	*ptr,
    int		col,
    int		mincol)
{
    int		len;
    int		scol;		/* starting column of the abbr. */
    int		j;
    char_u	*s;
    char_u	tb[MB_MAXBYTES + 4];
    mapblock_T	*mp;
#ifdef FEAT_LOCALMAP
    mapblock_T	*mp2;
#endif
#ifdef FEAT_MBYTE
    int		clen = 0;	/* length in characters */
#endif
    int		is_id = TRUE;
    int		vim_abbr;

    if (typebuf.tb_no_abbr_cnt)	/* abbrev. are not recursive */
	return FALSE;

    /* no remapping implies no abbreviation, except for CTRL-] */
    if ((KeyNoremap & (RM_NONE|RM_SCRIPT)) != 0 && c != Ctrl_RSB)
	return FALSE;

    /*
     * Check for word before the cursor: If it ends in a keyword char all
     * chars before it must be keyword chars or non-keyword chars, but not
     * white space. If it ends in a non-keyword char we accept any characters
     * before it except white space.
     */
    if (col == 0)				/* cannot be an abbr. */
	return FALSE;

#ifdef FEAT_MBYTE
    if (has_mbyte)
    {
	char_u *p;

	p = mb_prevptr(ptr, ptr + col);
	if (!vim_iswordp(p))
	    vim_abbr = TRUE;			/* Vim added abbr. */
	else
	{
	    vim_abbr = FALSE;			/* vi compatible abbr. */
	    if (p > ptr)
		is_id = vim_iswordp(mb_prevptr(ptr, p));
	}
	clen = 1;
	while (p > ptr + mincol)
	{
	    p = mb_prevptr(ptr, p);
	    if (vim_isspace(*p) || (!vim_abbr && is_id != vim_iswordp(p)))
	    {
		p += (*mb_ptr2len)(p);
		break;
	    }
	    ++clen;
	}
	scol = (int)(p - ptr);
    }
    else
#endif
    {
	if (!vim_iswordc(ptr[col - 1]))
	    vim_abbr = TRUE;			/* Vim added abbr. */
	else
	{
	    vim_abbr = FALSE;			/* vi compatible abbr. */
	    if (col > 1)
		is_id = vim_iswordc(ptr[col - 2]);
	}
	for (scol = col - 1; scol > 0 && !vim_isspace(ptr[scol - 1])
		&& (vim_abbr || is_id == vim_iswordc(ptr[scol - 1])); --scol)
	    ;
    }

    if (scol < mincol)
	scol = mincol;
    if (scol < col)		/* there is a word in front of the cursor */
    {
	ptr += scol;
	len = col - scol;
#ifdef FEAT_LOCALMAP
	mp = curbuf->b_first_abbr;
	mp2 = first_abbr;
	if (mp == NULL)
	{
	    mp = mp2;
	    mp2 = NULL;
	}
#else
	mp = first_abbr;
#endif
	for ( ; mp;
#ifdef FEAT_LOCALMAP
		mp->m_next == NULL ? (mp = mp2, mp2 = NULL) :
#endif
		(mp = mp->m_next))
	{
	    int		qlen = mp->m_keylen;
	    char_u	*q = mp->m_keys;
	    int		match;

	    if (vim_strbyte(mp->m_keys, K_SPECIAL) != NULL)
	    {
		/* might have CSI escaped mp->m_keys */
		q = vim_strsave(mp->m_keys);
		if (q != NULL)
		{
		    vim_unescape_csi(q);
		    qlen = (int)STRLEN(q);
		}
	    }

	    /* find entries with right mode and keys */
	    match =    (mp->m_mode & State)
		    && qlen == len
		    && !STRNCMP(q, ptr, (size_t)len);
	    if (q != mp->m_keys)
		vim_free(q);
	    if (match)
		break;
	}
	if (mp != NULL)
	{
	    /*
	     * Found a match:
	     * Insert the rest of the abbreviation in typebuf.tb_buf[].
	     * This goes from end to start.
	     *
	     * Characters 0x000 - 0x100: normal chars, may need CTRL-V,
	     * except K_SPECIAL: Becomes K_SPECIAL KS_SPECIAL KE_FILLER
	     * Characters where IS_SPECIAL() == TRUE: key codes, need
	     * K_SPECIAL. Other characters (with ABBR_OFF): don't use CTRL-V.
	     *
	     * Character CTRL-] is treated specially - it completes the
	     * abbreviation, but is not inserted into the input stream.
	     */
	    j = 0;
	    if (c != Ctrl_RSB)
	    {
					/* special key code, split up */
		if (IS_SPECIAL(c) || c == K_SPECIAL)
		{
		    tb[j++] = K_SPECIAL;
		    tb[j++] = K_SECOND(c);
		    tb[j++] = K_THIRD(c);
		}
		else
		{
		    if (c < ABBR_OFF && (c < ' ' || c > '~'))
			tb[j++] = Ctrl_V;	/* special char needs CTRL-V */
#ifdef FEAT_MBYTE
		    if (has_mbyte)
		    {
			/* if ABBR_OFF has been added, remove it here */
			if (c >= ABBR_OFF)
			    c -= ABBR_OFF;
			j += (*mb_char2bytes)(c, tb + j);
		    }
		    else
#endif
			tb[j++] = c;
		}
		tb[j] = NUL;
					/* insert the last typed char */
		(void)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    }
#ifdef FEAT_EVAL
	    if (mp->m_expr)
		s = eval_map_expr(mp->m_str, c);
	    else
#endif
		s = mp->m_str;
	    if (s != NULL)
	    {
					/* insert the to string */
		(void)ins_typebuf(s, mp->m_noremap, 0, TRUE, mp->m_silent);
					/* no abbrev. for these chars */
		typebuf.tb_no_abbr_cnt += (int)STRLEN(s) + j + 1;
#ifdef FEAT_EVAL
		if (mp->m_expr)
		    vim_free(s);
#endif
	    }

	    tb[0] = Ctrl_H;
	    tb[1] = NUL;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		len = clen;	/* Delete characters instead of bytes */
#endif
	    while (len-- > 0)		/* delete the from string */
		(void)ins_typebuf(tb, 1, 0, TRUE, mp->m_silent);
	    return TRUE;
	}
    }
    return FALSE;
}

#ifdef FEAT_EVAL
/*
 * Evaluate the RHS of a mapping or abbreviations and take care of escaping
 * special characters.
 */
    static char_u *
eval_map_expr(
    char_u	*str,
    int		c)	    /* NUL or typed character for abbreviation */
{
    char_u	*res;
    char_u	*p;
    char_u	*expr;
    char_u	*save_cmd;
    pos_T	save_cursor;
    int		save_msg_col;
    int		save_msg_row;

    /* Remove escaping of CSI, because "str" is in a format to be used as
     * typeahead. */
    expr = vim_strsave(str);
    if (expr == NULL)
	return NULL;
    vim_unescape_csi(expr);

    save_cmd = save_cmdline_alloc();
    if (save_cmd == NULL)
    {
	vim_free(expr);
	return NULL;
    }

    /* Forbid changing text or using ":normal" to avoid most of the bad side
     * effects.  Also restore the cursor position. */
    ++textlock;
    ++ex_normal_lock;
    set_vim_var_char(c);  /* set v:char to the typed character */
    save_cursor = curwin->w_cursor;
    save_msg_col = msg_col;
    save_msg_row = msg_row;
    p = eval_to_string(expr, NULL, FALSE);
    --textlock;
    --ex_normal_lock;
    curwin->w_cursor = save_cursor;
    msg_col = save_msg_col;
    msg_row = save_msg_row;

    restore_cmdline_alloc(save_cmd);
    vim_free(expr);

    if (p == NULL)
	return NULL;
    /* Escape CSI in the result to be able to use the string as typeahead. */
    res = vim_strsave_escape_csi(p);
    vim_free(p);

    return res;
}
#endif

/*
 * Copy "p" to allocated memory, escaping K_SPECIAL and CSI so that the result
 * can be put in the typeahead buffer.
 * Returns NULL when out of memory.
 */
    char_u *
vim_strsave_escape_csi(
    char_u *p)
{
    char_u	*res;
    char_u	*s, *d;

    /* Need a buffer to hold up to three times as much. */
    res = alloc((unsigned)(STRLEN(p) * 3) + 1);
    if (res != NULL)
    {
	d = res;
	for (s = p; *s != NUL; )
	{
	    if (s[0] == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	    {
		/* Copy special key unmodified. */
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
	    }
	    else
	    {
#ifdef FEAT_MBYTE
		int len  = mb_char2len(PTR2CHAR(s));
		int len2 = mb_ptr2len(s);
#endif
		/* Add character, possibly multi-byte to destination, escaping
		 * CSI and K_SPECIAL. */
		d = add_char2buf(PTR2CHAR(s), d);
#ifdef FEAT_MBYTE
		while (len < len2)
		{
		    /* add following combining char */
		    d = add_char2buf(PTR2CHAR(s + len), d);
		    len += mb_char2len(PTR2CHAR(s + len));
		}
#endif
		mb_ptr_adv(s);
	    }
	}
	*d = NUL;
    }
    return res;
}

/*
 * Remove escaping from CSI and K_SPECIAL characters.  Reverse of
 * vim_strsave_escape_csi().  Works in-place.
 */
    void
vim_unescape_csi(char_u *p)
{
    char_u	*s = p, *d = p;

    while (*s != NUL)
    {
	if (s[0] == K_SPECIAL && s[1] == KS_SPECIAL && s[2] == KE_FILLER)
	{
	    *d++ = K_SPECIAL;
	    s += 3;
	}
	else if ((s[0] == K_SPECIAL || s[0] == CSI)
				   && s[1] == KS_EXTRA && s[2] == (int)KE_CSI)
	{
	    *d++ = CSI;
	    s += 3;
	}
	else
	    *d++ = *s++;
    }
    *d = NUL;
}

/*
 * Write map commands for the current mappings to an .exrc file.
 * Return FAIL on error, OK otherwise.
 */
    int
makemap(
    FILE	*fd,
    buf_T	*buf)	    /* buffer for local mappings or NULL */
{
    mapblock_T	*mp;
    char_u	c1, c2, c3;
    char_u	*p;
    char	*cmd;
    int		abbr;
    int		hash;
    int		did_cpo = FALSE;
    int		i;

    validate_maphash();

    /*
     * Do the loop twice: Once for mappings, once for abbreviations.
     * Then loop over all map hash lists.
     */
    for (abbr = 0; abbr < 2; ++abbr)
	for (hash = 0; hash < 256; ++hash)
	{
	    if (abbr)
	    {
		if (hash > 0)		/* there is only one abbr list */
		    break;
#ifdef FEAT_LOCALMAP
		if (buf != NULL)
		    mp = buf->b_first_abbr;
		else
#endif
		    mp = first_abbr;
	    }
	    else
	    {
#ifdef FEAT_LOCALMAP
		if (buf != NULL)
		    mp = buf->b_maphash[hash];
		else
#endif
		    mp = maphash[hash];
	    }

	    for ( ; mp; mp = mp->m_next)
	    {
		/* skip script-local mappings */
		if (mp->m_noremap == REMAP_SCRIPT)
		    continue;

		/* skip mappings that contain a <SNR> (script-local thing),
		 * they probably don't work when loaded again */
		for (p = mp->m_str; *p != NUL; ++p)
		    if (p[0] == K_SPECIAL && p[1] == KS_EXTRA
						       && p[2] == (int)KE_SNR)
			break;
		if (*p != NUL)
		    continue;

		/* It's possible to create a mapping and then ":unmap" certain
		 * modes.  We recreate this here by mapping the individual
		 * modes, which requires up to three of them. */
		c1 = NUL;
		c2 = NUL;
		c3 = NUL;
		if (abbr)
		    cmd = "abbr";
		else
		    cmd = "map";
		switch (mp->m_mode)
		{
		    case NORMAL + VISUAL + SELECTMODE + OP_PENDING:
			break;
		    case NORMAL:
			c1 = 'n';
			break;
		    case VISUAL:
			c1 = 'x';
			break;
		    case SELECTMODE:
			c1 = 's';
			break;
		    case OP_PENDING:
			c1 = 'o';
			break;
		    case NORMAL + VISUAL:
			c1 = 'n';
			c2 = 'x';
			break;
		    case NORMAL + SELECTMODE:
			c1 = 'n';
			c2 = 's';
			break;
		    case NORMAL + OP_PENDING:
			c1 = 'n';
			c2 = 'o';
			break;
		    case VISUAL + SELECTMODE:
			c1 = 'v';
			break;
		    case VISUAL + OP_PENDING:
			c1 = 'x';
			c2 = 'o';
			break;
		    case SELECTMODE + OP_PENDING:
			c1 = 's';
			c2 = 'o';
			break;
		    case NORMAL + VISUAL + SELECTMODE:
			c1 = 'n';
			c2 = 'v';
			break;
		    case NORMAL + VISUAL + OP_PENDING:
			c1 = 'n';
			c2 = 'x';
			c3 = 'o';
			break;
		    case NORMAL + SELECTMODE + OP_PENDING:
			c1 = 'n';
			c2 = 's';
			c3 = 'o';
			break;
		    case VISUAL + SELECTMODE + OP_PENDING:
			c1 = 'v';
			c2 = 'o';
			break;
		    case CMDLINE + INSERT:
			if (!abbr)
			    cmd = "map!";
			break;
		    case CMDLINE:
			c1 = 'c';
			break;
		    case INSERT:
			c1 = 'i';
			break;
		    case LANGMAP:
			c1 = 'l';
			break;
		    default:
			EMSG(_("E228: makemap: Illegal mode"));
			return FAIL;
		}
		do	/* do this twice if c2 is set, 3 times with c3 */
		{
		    /* When outputting <> form, need to make sure that 'cpo'
		     * is set to the Vim default. */
		    if (!did_cpo)
		    {
			if (*mp->m_str == NUL)		/* will use <Nop> */
			    did_cpo = TRUE;
			else
			    for (i = 0; i < 2; ++i)
				for (p = (i ? mp->m_str : mp->m_keys); *p; ++p)
				    if (*p == K_SPECIAL || *p == NL)
					did_cpo = TRUE;
			if (did_cpo)
			{
			    if (fprintf(fd, "let s:cpo_save=&cpo") < 0
				    || put_eol(fd) < 0
				    || fprintf(fd, "set cpo&vim") < 0
				    || put_eol(fd) < 0)
				return FAIL;
			}
		    }
		    if (c1 && putc(c1, fd) < 0)
			return FAIL;
		    if (mp->m_noremap != REMAP_YES && fprintf(fd, "nore") < 0)
			return FAIL;
		    if (fputs(cmd, fd) < 0)
			return FAIL;
		    if (buf != NULL && fputs(" <buffer>", fd) < 0)
			return FAIL;
		    if (mp->m_nowait && fputs(" <nowait>", fd) < 0)
			return FAIL;
		    if (mp->m_silent && fputs(" <silent>", fd) < 0)
			return FAIL;
#ifdef FEAT_EVAL
		    if (mp->m_noremap == REMAP_SCRIPT
						 && fputs("<script>", fd) < 0)
			return FAIL;
		    if (mp->m_expr && fputs(" <expr>", fd) < 0)
			return FAIL;
#endif

		    if (       putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_keys, 0) == FAIL
			    || putc(' ', fd) < 0
			    || put_escstr(fd, mp->m_str, 1) == FAIL
			    || put_eol(fd) < 0)
			return FAIL;
		    c1 = c2;
		    c2 = c3;
		    c3 = NUL;
		} while (c1 != NUL);
	    }
	}

    if (did_cpo)
	if (fprintf(fd, "let &cpo=s:cpo_save") < 0
		|| put_eol(fd) < 0
		|| fprintf(fd, "unlet s:cpo_save") < 0
		|| put_eol(fd) < 0)
	    return FAIL;
    return OK;
}

/*
 * write escape string to file
 * "what": 0 for :map lhs, 1 for :map rhs, 2 for :set
 *
 * return FAIL for failure, OK otherwise
 */
    int
put_escstr(FILE *fd, char_u *strstart, int what)
{
    char_u	*str = strstart;
    int		c;
    int		modifiers;

    /* :map xx <Nop> */
    if (*str == NUL && what == 1)
    {
	if (fprintf(fd, "<Nop>") < 0)
	    return FAIL;
	return OK;
    }

    for ( ; *str != NUL; ++str)
    {
#ifdef FEAT_MBYTE
	char_u	*p;

	/* Check for a multi-byte character, which may contain escaped
	 * K_SPECIAL and CSI bytes */
	p = mb_unescape(&str);
	if (p != NULL)
	{
	    while (*p != NUL)
		if (fputc(*p++, fd) < 0)
		    return FAIL;
	    --str;
	    continue;
	}
#endif

	c = *str;
	/*
	 * Special key codes have to be translated to be able to make sense
	 * when they are read back.
	 */
	if (c == K_SPECIAL && what != 2)
	{
	    modifiers = 0x0;
	    if (str[1] == KS_MODIFIER)
	    {
		modifiers = str[2];
		str += 3;
		c = *str;
	    }
	    if (c == K_SPECIAL)
	    {
		c = TO_SPECIAL(str[1], str[2]);
		str += 2;
	    }
	    if (IS_SPECIAL(c) || modifiers)	/* special key */
	    {
		if (fputs((char *)get_special_key_name(c, modifiers), fd) < 0)
		    return FAIL;
		continue;
	    }
	}

	/*
	 * A '\n' in a map command should be written as <NL>.
	 * A '\n' in a set command should be written as \^V^J.
	 */
	if (c == NL)
	{
	    if (what == 2)
	    {
		if (fprintf(fd, IF_EB("\\\026\n", "\\" CTRL_V_STR "\n")) < 0)
		    return FAIL;
	    }
	    else
	    {
		if (fprintf(fd, "<NL>") < 0)
		    return FAIL;
	    }
	    continue;
	}

	/*
	 * Some characters have to be escaped with CTRL-V to
	 * prevent them from misinterpreted in DoOneCmd().
	 * A space, Tab and '"' has to be escaped with a backslash to
	 * prevent it to be misinterpreted in do_set().
	 * A space has to be escaped with a CTRL-V when it's at the start of a
	 * ":map" rhs.
	 * A '<' has to be escaped with a CTRL-V to prevent it being
	 * interpreted as the start of a special key name.
	 * A space in the lhs of a :map needs a CTRL-V.
	 */
	if (what == 2 && (vim_iswhite(c) || c == '"' || c == '\\'))
	{
	    if (putc('\\', fd) < 0)
		return FAIL;
	}
	else if (c < ' ' || c > '~' || c == '|'
		|| (what == 0 && c == ' ')
		|| (what == 1 && str == strstart && c == ' ')
		|| (what != 2 && c == '<'))
	{
	    if (putc(Ctrl_V, fd) < 0)
		return FAIL;
	}
	if (putc(c, fd) < 0)
	    return FAIL;
    }
    return OK;
}

/*
 * Check all mappings for the presence of special key codes.
 * Used after ":set term=xxx".
 */
    void
check_map_keycodes(void)
{
    mapblock_T	*mp;
    char_u	*p;
    int		i;
    char_u	buf[3];
    char_u	*save_name;
    int		abbr;
    int		hash;
#ifdef FEAT_LOCALMAP
    buf_T	*bp;
#endif

    validate_maphash();
    save_name = sourcing_name;
    sourcing_name = (char_u *)"mappings"; /* avoids giving error messages */

#ifdef FEAT_LOCALMAP
    /* This this once for each buffer, and then once for global
     * mappings/abbreviations with bp == NULL */
    for (bp = firstbuf; ; bp = bp->b_next)
    {
#endif
	/*
	 * Do the loop twice: Once for mappings, once for abbreviations.
	 * Then loop over all map hash lists.
	 */
	for (abbr = 0; abbr <= 1; ++abbr)
	    for (hash = 0; hash < 256; ++hash)
	    {
		if (abbr)
		{
		    if (hash)	    /* there is only one abbr list */
			break;
#ifdef FEAT_LOCALMAP
		    if (bp != NULL)
			mp = bp->b_first_abbr;
		    else
#endif
			mp = first_abbr;
		}
		else
		{
#ifdef FEAT_LOCALMAP
		    if (bp != NULL)
			mp = bp->b_maphash[hash];
		    else
#endif
			mp = maphash[hash];
		}
		for ( ; mp != NULL; mp = mp->m_next)
		{
		    for (i = 0; i <= 1; ++i)	/* do this twice */
		    {
			if (i == 0)
			    p = mp->m_keys;	/* once for the "from" part */
			else
			    p = mp->m_str;	/* and once for the "to" part */
			while (*p)
			{
			    if (*p == K_SPECIAL)
			    {
				++p;
				if (*p < 128)   /* for "normal" tcap entries */
				{
				    buf[0] = p[0];
				    buf[1] = p[1];
				    buf[2] = NUL;
				    (void)add_termcap_entry(buf, FALSE);
				}
				++p;
			    }
			    ++p;
			}
		    }
		}
	    }
#ifdef FEAT_LOCALMAP
	if (bp == NULL)
	    break;
    }
#endif
    sourcing_name = save_name;
}

#if defined(FEAT_EVAL) || defined(PROTO)
/*
 * Check the string "keys" against the lhs of all mappings.
 * Return pointer to rhs of mapping (mapblock->m_str).
 * NULL when no mapping found.
 */
    char_u *
check_map(
    char_u	*keys,
    int		mode,
    int		exact,		/* require exact match */
    int		ign_mod,	/* ignore preceding modifier */
    int		abbr,		/* do abbreviations */
    mapblock_T	**mp_ptr,	/* return: pointer to mapblock or NULL */
    int		*local_ptr)	/* return: buffer-local mapping or NULL */
{
    int		hash;
    int		len, minlen;
    mapblock_T	*mp;
    char_u	*s;
#ifdef FEAT_LOCALMAP
    int		local;
#endif

    validate_maphash();

    len = (int)STRLEN(keys);
#ifdef FEAT_LOCALMAP
    for (local = 1; local >= 0; --local)
#endif
	/* loop over all hash lists */
	for (hash = 0; hash < 256; ++hash)
	{
	    if (abbr)
	    {
		if (hash > 0)		/* there is only one list. */
		    break;
#ifdef FEAT_LOCALMAP
		if (local)
		    mp = curbuf->b_first_abbr;
		else
#endif
		    mp = first_abbr;
	    }
#ifdef FEAT_LOCALMAP
	    else if (local)
		mp = curbuf->b_maphash[hash];
#endif
	    else
		mp = maphash[hash];
	    for ( ; mp != NULL; mp = mp->m_next)
	    {
		/* skip entries with wrong mode, wrong length and not matching
		 * ones */
		if ((mp->m_mode & mode) && (!exact || mp->m_keylen == len))
		{
		    if (len > mp->m_keylen)
			minlen = mp->m_keylen;
		    else
			minlen = len;
		    s = mp->m_keys;
		    if (ign_mod && s[0] == K_SPECIAL && s[1] == KS_MODIFIER
							       && s[2] != NUL)
		    {
			s += 3;
			if (len > mp->m_keylen - 3)
			    minlen = mp->m_keylen - 3;
		    }
		    if (STRNCMP(s, keys, minlen) == 0)
		    {
			if (mp_ptr != NULL)
			    *mp_ptr = mp;
			if (local_ptr != NULL)
#ifdef FEAT_LOCALMAP
			    *local_ptr = local;
#else
			    *local_ptr = 0;
#endif
			return mp->m_str;
		    }
		}
	    }
	}

    return NULL;
}
#endif

#if defined(MSWIN) || defined(MACOS)

#define VIS_SEL	(VISUAL+SELECTMODE)	/* abbreviation */

/*
 * Default mappings for some often used keys.
 */
static struct initmap
{
    char_u	*arg;
    int		mode;
} initmappings[] =
{
#if defined(MSWIN)
	/* Use the Windows (CUA) keybindings. */
# ifdef FEAT_GUI
	/* paste, copy and cut */
	{(char_u *)"<S-Insert> \"*P", NORMAL},
	{(char_u *)"<S-Insert> \"-d\"*P", VIS_SEL},
	{(char_u *)"<S-Insert> <C-R><C-O>*", INSERT+CMDLINE},
	{(char_u *)"<C-Insert> \"*y", VIS_SEL},
	{(char_u *)"<S-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-Del> \"*d", VIS_SEL},
	{(char_u *)"<C-X> \"*d", VIS_SEL},
	/* Missing: CTRL-C (cancel) and CTRL-V (block selection) */
# else
	{(char_u *)"\316w <C-Home>", NORMAL+VIS_SEL},
	{(char_u *)"\316w <C-Home>", INSERT+CMDLINE},
	{(char_u *)"\316u <C-End>", NORMAL+VIS_SEL},
	{(char_u *)"\316u <C-End>", INSERT+CMDLINE},

	/* paste, copy and cut */
#  ifdef FEAT_CLIPBOARD
	{(char_u *)"\316\324 \"*P", NORMAL},	    /* SHIFT-Insert is "*P */
	{(char_u *)"\316\324 \"-d\"*P", VIS_SEL},   /* SHIFT-Insert is "-d"*P */
	{(char_u *)"\316\324 \022\017*", INSERT},  /* SHIFT-Insert is ^R^O* */
	{(char_u *)"\316\325 \"*y", VIS_SEL},	    /* CTRL-Insert is "*y */
	{(char_u *)"\316\327 \"*d", VIS_SEL},	    /* SHIFT-Del is "*d */
	{(char_u *)"\316\330 \"*d", VIS_SEL},	    /* CTRL-Del is "*d */
	{(char_u *)"\030 \"-d", VIS_SEL},	    /* CTRL-X is "-d */
#  else
	{(char_u *)"\316\324 P", NORMAL},	    /* SHIFT-Insert is P */
	{(char_u *)"\316\324 \"-dP", VIS_SEL},	    /* SHIFT-Insert is "-dP */
	{(char_u *)"\316\324 \022\017\"", INSERT}, /* SHIFT-Insert is ^R^O" */
	{(char_u *)"\316\325 y", VIS_SEL},	    /* CTRL-Insert is y */
	{(char_u *)"\316\327 d", VIS_SEL},	    /* SHIFT-Del is d */
	{(char_u *)"\316\330 d", VIS_SEL},	    /* CTRL-Del is d */
#  endif
# endif
#endif

#if defined(MACOS)
	/* Use the Standard MacOS binding. */
	/* paste, copy and cut */
	{(char_u *)"<D-v> \"*P", NORMAL},
	{(char_u *)"<D-v> \"-d\"*P", VIS_SEL},
	{(char_u *)"<D-v> <C-R>*", INSERT+CMDLINE},
	{(char_u *)"<D-c> \"*y", VIS_SEL},
	{(char_u *)"<D-x> \"*d", VIS_SEL},
	{(char_u *)"<Backspace> \"-d", VIS_SEL},
#endif
};

# undef VIS_SEL
#endif

/*
 * Set up default mappings.
 */
    void
init_mappings(void)
{
#if defined(MSWIN) ||defined(MACOS)
    int		i;

    for (i = 0; i < (int)(sizeof(initmappings) / sizeof(struct initmap)); ++i)
	add_map(initmappings[i].arg, initmappings[i].mode);
#endif
}

#if defined(MSWIN) || defined(FEAT_CMDWIN) || defined(MACOS) || defined(PROTO)
/*
 * Add a mapping "map" for mode "mode".
 * Need to put string in allocated memory, because do_map() will modify it.
 */
    void
add_map(char_u *map, int mode)
{
    char_u	*s;
    char_u	*cpo_save = p_cpo;

    p_cpo = (char_u *)"";	/* Allow <> notation */
    s = vim_strsave(map);
    if (s != NULL)
    {
	(void)do_map(0, s, mode, FALSE);
	vim_free(s);
    }
    p_cpo = cpo_save;
}
#endif

//						↓↓↓VULNERABLE LINES↓↓↓

// 295,4;295,11

