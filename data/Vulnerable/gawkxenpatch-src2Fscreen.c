/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * screen.c: code for displaying on the screen
 *
 * Output to the screen (console, terminal emulator or GUI window) is minimized
 * by remembering what is already on the screen, and only updating the parts
 * that changed.
 *
 * ScreenLines[off]  Contains a copy of the whole screen, as it is currently
 *		     displayed (excluding text written by external commands).
 * ScreenAttrs[off]  Contains the associated attributes.
 * LineOffset[row]   Contains the offset into ScreenLines*[] and ScreenAttrs[]
 *		     for each line.
 * LineWraps[row]    Flag for each line whether it wraps to the next line.
 *
 * For double-byte characters, two consecutive bytes in ScreenLines[] can form
 * one character which occupies two display cells.
 * For UTF-8 a multi-byte character is converted to Unicode and stored in
 * ScreenLinesUC[].  ScreenLines[] contains the first byte only.  For an ASCII
 * character without composing chars ScreenLinesUC[] will be 0 and
 * ScreenLinesC[][] is not used.  When the character occupies two display
 * cells the next byte in ScreenLines[] is 0.
 * ScreenLinesC[][] contain up to 'maxcombine' composing characters
 * (drawn on top of the first character).  There is 0 after the last one used.
 * ScreenLines2[] is only used for euc-jp to store the second byte if the
 * first byte is 0x8e (single-width character).
 *
 * The screen_*() functions write to the screen and handle updating
 * ScreenLines[].
 *
 * update_screen() is the function that updates all windows and status lines.
 * It is called form the main loop when must_redraw is non-zero.  It may be
 * called from other places when an immediate screen update is needed.
 *
 * The part of the buffer that is displayed in a window is set with:
 * - w_topline (first buffer line in window)
 * - w_topfill (filler lines above the first line)
 * - w_leftcol (leftmost window cell in window),
 * - w_skipcol (skipped window cells of first line)
 *
 * Commands that only move the cursor around in a window, do not need to take
 * action to update the display.  The main loop will check if w_topline is
 * valid and update it (scroll the window) when needed.
 *
 * Commands that scroll a window change w_topline and must call
 * check_cursor() to move the cursor into the visible part of the window, and
 * call redraw_later(VALID) to have the window displayed by update_screen()
 * later.
 *
 * Commands that change text in the buffer must call changed_bytes() or
 * changed_lines() to mark the area that changed and will require updating
 * later.  The main loop will call update_screen(), which will update each
 * window that shows the changed buffer.  This assumes text above the change
 * can remain displayed as it is.  Text after the change may need updating for
 * scrolling, folding and syntax highlighting.
 *
 * Commands that change how a window is displayed (e.g., setting 'list') or
 * invalidate the contents of a window in another way (e.g., change fold
 * settings), must call redraw_later(NOT_VALID) to have the whole window
 * redisplayed by update_screen() later.
 *
 * Commands that change how a buffer is displayed (e.g., setting 'tabstop')
 * must call redraw_curbuf_later(NOT_VALID) to have all the windows for the
 * buffer redisplayed by update_screen() later.
 *
 * Commands that change highlighting and possibly cause a scroll too must call
 * redraw_later(SOME_VALID) to update the whole window but still use scrolling
 * to avoid redrawing everything.  But the length of displayed lines must not
 * change, use NOT_VALID then.
 *
 * Commands that move the window position must call redraw_later(NOT_VALID).
 * TODO: should minimize redrawing by scrolling when possible.
 *
 * Commands that change everything (e.g., resizing the screen) must call
 * redraw_all_later(NOT_VALID) or redraw_all_later(CLEAR).
 *
 * Things that are handled indirectly:
 * - When messages scroll the screen up, msg_scrolled will be set and
 *   update_screen() called to redraw.
 */

#include "vim.h"

#define MB_FILLER_CHAR '<'  /* character used when a double-width character
			     * doesn't fit. */

/*
 * The attributes that are actually active for writing to the screen.
 */
static int	screen_attr = 0;

/*
 * Positioning the cursor is reduced by remembering the last position.
 * Mostly used by windgoto() and screen_char().
 */
static int	screen_cur_row, screen_cur_col;	/* last known cursor position */

#ifdef FEAT_SEARCH_EXTRA
static match_T search_hl;	/* used for 'hlsearch' highlight matching */
#endif

#ifdef FEAT_FOLDING
static foldinfo_T win_foldinfo;	/* info for 'foldcolumn' */
static int compute_foldcolumn(win_T *wp, int col);
#endif

/*
 * Buffer for one screen line (characters and attributes).
 */
static schar_T	*current_ScreenLine;

static void win_update(win_T *wp);
static void win_draw_end(win_T *wp, int c1, int c2, int row, int endrow, hlf_T hl);
#ifdef FEAT_FOLDING
static void fold_line(win_T *wp, long fold_count, foldinfo_T *foldinfo, linenr_T lnum, int row);
static void fill_foldcolumn(char_u *p, win_T *wp, int closed, linenr_T lnum);
static void copy_text_attr(int off, char_u *buf, int len, int attr);
#endif
static int win_line(win_T *, linenr_T, int, int, int nochange);
static int char_needs_redraw(int off_from, int off_to, int cols);
#ifdef FEAT_RIGHTLEFT
static void screen_line(int row, int coloff, int endcol, int clear_width, int rlflag);
# define SCREEN_LINE(r, o, e, c, rl)    screen_line((r), (o), (e), (c), (rl))
#else
static void screen_line(int row, int coloff, int endcol, int clear_width);
# define SCREEN_LINE(r, o, e, c, rl)    screen_line((r), (o), (e), (c))
#endif
#ifdef FEAT_WINDOWS
static void draw_vsep_win(win_T *wp, int row);
#endif
#ifdef FEAT_STL_OPT
static void redraw_custom_statusline(win_T *wp);
#endif
#ifdef FEAT_SEARCH_EXTRA
# define SEARCH_HL_PRIORITY 0
static void start_search_hl(void);
static void end_search_hl(void);
static void init_search_hl(win_T *wp);
static void prepare_search_hl(win_T *wp, linenr_T lnum);
static void next_search_hl(win_T *win, match_T *shl, linenr_T lnum, colnr_T mincol, matchitem_T *cur);
static int next_search_hl_pos(match_T *shl, linenr_T lnum, posmatch_T *pos, colnr_T mincol);
#endif
static void screen_start_highlight(int attr);
static void screen_char(unsigned off, int row, int col);
#ifdef FEAT_MBYTE
static void screen_char_2(unsigned off, int row, int col);
#endif
static void screenclear2(void);
static void lineclear(unsigned off, int width);
static void lineinvalid(unsigned off, int width);
#ifdef FEAT_WINDOWS
static void linecopy(int to, int from, win_T *wp);
static void redraw_block(int row, int end, win_T *wp);
#endif
static int win_do_lines(win_T *wp, int row, int line_count, int mayclear, int del);
static void win_rest_invalid(win_T *wp);
static void msg_pos_mode(void);
static void recording_mode(int attr);
#if defined(FEAT_WINDOWS)
static void draw_tabline(void);
#endif
#if defined(FEAT_WINDOWS) || defined(FEAT_WILDMENU) || defined(FEAT_STL_OPT)
static int fillchar_status(int *attr, int is_curwin);
#endif
#ifdef FEAT_WINDOWS
static int fillchar_vsep(int *attr);
#endif
#ifdef FEAT_STL_OPT
static void win_redr_custom(win_T *wp, int draw_ruler);
#endif
#ifdef FEAT_CMDL_INFO
static void win_redr_ruler(win_T *wp, int always);
#endif

#if defined(FEAT_CLIPBOARD) || defined(FEAT_WINDOWS)
/* Ugly global: overrule attribute used by screen_char() */
static int screen_char_attr = 0;
#endif

/*
 * Redraw the current window later, with update_screen(type).
 * Set must_redraw only if not already set to a higher value.
 * e.g. if must_redraw is CLEAR, type NOT_VALID will do nothing.
 */
    void
redraw_later(int type)
{
    redraw_win_later(curwin, type);
}

    void
redraw_win_later(
    win_T	*wp,
    int		type)
{
    if (wp->w_redr_type < type)
    {
	wp->w_redr_type = type;
	if (type >= NOT_VALID)
	    wp->w_lines_valid = 0;
	if (must_redraw < type)	/* must_redraw is the maximum of all windows */
	    must_redraw = type;
    }
}

/*
 * Force a complete redraw later.  Also resets the highlighting.  To be used
 * after executing a shell command that messes up the screen.
 */
    void
redraw_later_clear(void)
{
    redraw_all_later(CLEAR);
#ifdef FEAT_GUI
    if (gui.in_use)
	/* Use a code that will reset gui.highlight_mask in
	 * gui_stop_highlight(). */
	screen_attr = HL_ALL + 1;
    else
#endif
	/* Use attributes that is very unlikely to appear in text. */
	screen_attr = HL_BOLD | HL_UNDERLINE | HL_INVERSE;
}

/*
 * Mark all windows to be redrawn later.
 */
    void
redraw_all_later(int type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	redraw_win_later(wp, type);
    }
}

/*
 * Mark all windows that are editing the current buffer to be updated later.
 */
    void
redraw_curbuf_later(int type)
{
    redraw_buf_later(curbuf, type);
}

    void
redraw_buf_later(buf_T *buf, int type)
{
    win_T	*wp;

    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_buffer == buf)
	    redraw_win_later(wp, type);
    }
}

/*
 * Redraw as soon as possible.  When the command line is not scrolled redraw
 * right away and restore what was on the command line.
 * Return a code indicating what happened.
 */
    int
redraw_asap(int type)
{
    int		rows;
    int		cols = screen_Columns;
    int		r;
    int		ret = 0;
    schar_T	*screenline;	/* copy from ScreenLines[] */
    sattr_T	*screenattr;	/* copy from ScreenAttrs[] */
#ifdef FEAT_MBYTE
    int		i;
    u8char_T	*screenlineUC = NULL;	/* copy from ScreenLinesUC[] */
    u8char_T	*screenlineC[MAX_MCO];	/* copy from ScreenLinesC[][] */
    schar_T	*screenline2 = NULL;	/* copy from ScreenLines2[] */
#endif

    redraw_later(type);
    if (msg_scrolled || (State != NORMAL && State != NORMAL_BUSY) || exiting)
	return ret;

    /* Allocate space to save the text displayed in the command line area. */
    rows = screen_Rows - cmdline_row;
    screenline = (schar_T *)lalloc(
			   (long_u)(rows * cols * sizeof(schar_T)), FALSE);
    screenattr = (sattr_T *)lalloc(
			   (long_u)(rows * cols * sizeof(sattr_T)), FALSE);
    if (screenline == NULL || screenattr == NULL)
	ret = 2;
#ifdef FEAT_MBYTE
    if (enc_utf8)
    {
	screenlineUC = (u8char_T *)lalloc(
			  (long_u)(rows * cols * sizeof(u8char_T)), FALSE);
	if (screenlineUC == NULL)
	    ret = 2;
	for (i = 0; i < p_mco; ++i)
	{
	    screenlineC[i] = (u8char_T *)lalloc(
			  (long_u)(rows * cols * sizeof(u8char_T)), FALSE);
	    if (screenlineC[i] == NULL)
		ret = 2;
	}
    }
    if (enc_dbcs == DBCS_JPNU)
    {
	screenline2 = (schar_T *)lalloc(
			   (long_u)(rows * cols * sizeof(schar_T)), FALSE);
	if (screenline2 == NULL)
	    ret = 2;
    }
#endif

    if (ret != 2)
    {
	/* Save the text displayed in the command line area. */
	for (r = 0; r < rows; ++r)
	{
	    mch_memmove(screenline + r * cols,
			ScreenLines + LineOffset[cmdline_row + r],
			(size_t)cols * sizeof(schar_T));
	    mch_memmove(screenattr + r * cols,
			ScreenAttrs + LineOffset[cmdline_row + r],
			(size_t)cols * sizeof(sattr_T));
#ifdef FEAT_MBYTE
	    if (enc_utf8)
	    {
		mch_memmove(screenlineUC + r * cols,
			    ScreenLinesUC + LineOffset[cmdline_row + r],
			    (size_t)cols * sizeof(u8char_T));
		for (i = 0; i < p_mco; ++i)
		    mch_memmove(screenlineC[i] + r * cols,
				ScreenLinesC[i] + LineOffset[cmdline_row + r],
				(size_t)cols * sizeof(u8char_T));
	    }
	    if (enc_dbcs == DBCS_JPNU)
		mch_memmove(screenline2 + r * cols,
			    ScreenLines2 + LineOffset[cmdline_row + r],
			    (size_t)cols * sizeof(schar_T));
#endif
	}

	update_screen(0);
	ret = 3;

	if (must_redraw == 0)
	{
	    int	off = (int)(current_ScreenLine - ScreenLines);

	    /* Restore the text displayed in the command line area. */
	    for (r = 0; r < rows; ++r)
	    {
		mch_memmove(current_ScreenLine,
			    screenline + r * cols,
			    (size_t)cols * sizeof(schar_T));
		mch_memmove(ScreenAttrs + off,
			    screenattr + r * cols,
			    (size_t)cols * sizeof(sattr_T));
#ifdef FEAT_MBYTE
		if (enc_utf8)
		{
		    mch_memmove(ScreenLinesUC + off,
				screenlineUC + r * cols,
				(size_t)cols * sizeof(u8char_T));
		    for (i = 0; i < p_mco; ++i)
			mch_memmove(ScreenLinesC[i] + off,
				    screenlineC[i] + r * cols,
				    (size_t)cols * sizeof(u8char_T));
		}
		if (enc_dbcs == DBCS_JPNU)
		    mch_memmove(ScreenLines2 + off,
				screenline2 + r * cols,
				(size_t)cols * sizeof(schar_T));
#endif
		SCREEN_LINE(cmdline_row + r, 0, cols, cols, FALSE);
	    }
	    ret = 4;
	}
    }

    vim_free(screenline);
    vim_free(screenattr);
#ifdef FEAT_MBYTE
    if (enc_utf8)
    {
	vim_free(screenlineUC);
	for (i = 0; i < p_mco; ++i)
	    vim_free(screenlineC[i]);
    }
    if (enc_dbcs == DBCS_JPNU)
	vim_free(screenline2);
#endif

    /* Show the intro message when appropriate. */
    maybe_intro_message();

    setcursor();

    return ret;
}

/*
 * Invoked after an asynchronous callback is called.
 * If an echo command was used the cursor needs to be put back where
 * it belongs. If highlighting was changed a redraw is needed.
 */
    void
redraw_after_callback()
{
    if (State == HITRETURN || State == ASKMORE)
	; /* do nothing */
    else if (State & CMDLINE)
	redrawcmdline();
    else if ((State & NORMAL) || (State & INSERT))
    {
	update_screen(0);
	setcursor();
    }
    cursor_on();
    out_flush();
#ifdef FEAT_GUI
    if (gui.in_use)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
#endif
}

/*
 * Changed something in the current window, at buffer line "lnum", that
 * requires that line and possibly other lines to be redrawn.
 * Used when entering/leaving Insert mode with the cursor on a folded line.
 * Used to remove the "$" from a change command.
 * Note that when also inserting/deleting lines w_redraw_top and w_redraw_bot
 * may become invalid and the whole window will have to be redrawn.
 */
    void
redrawWinline(
    linenr_T	lnum,
    int		invalid UNUSED)	/* window line height is invalid now */
{
#ifdef FEAT_FOLDING
    int		i;
#endif

    if (curwin->w_redraw_top == 0 || curwin->w_redraw_top > lnum)
	curwin->w_redraw_top = lnum;
    if (curwin->w_redraw_bot == 0 || curwin->w_redraw_bot < lnum)
	curwin->w_redraw_bot = lnum;
    redraw_later(VALID);

#ifdef FEAT_FOLDING
    if (invalid)
    {
	/* A w_lines[] entry for this lnum has become invalid. */
	i = find_wl_entry(curwin, lnum);
	if (i >= 0)
	    curwin->w_lines[i].wl_valid = FALSE;
    }
#endif
}

/*
 * update all windows that are editing the current buffer
 */
    void
update_curbuf(int type)
{
    redraw_curbuf_later(type);
    update_screen(type);
}

/*
 * update_screen()
 *
 * Based on the current value of curwin->w_topline, transfer a screenfull
 * of stuff from Filemem to ScreenLines[], and update curwin->w_botline.
 */
    void
update_screen(int type)
{
    win_T	*wp;
    static int	did_intro = FALSE;
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    int		did_one;
#endif

    /* Don't do anything if the screen structures are (not yet) valid. */
    if (!screen_valid(TRUE))
	return;

    if (must_redraw)
    {
	if (type < must_redraw)	    /* use maximal type */
	    type = must_redraw;

	/* must_redraw is reset here, so that when we run into some weird
	 * reason to redraw while busy redrawing (e.g., asynchronous
	 * scrolling), or update_topline() in win_update() will cause a
	 * scroll, the screen will be redrawn later or in win_update(). */
	must_redraw = 0;
    }

    /* Need to update w_lines[]. */
    if (curwin->w_lines_valid == 0 && type < NOT_VALID)
	type = NOT_VALID;

    /* Postpone the redrawing when it's not needed and when being called
     * recursively. */
    if (!redrawing() || updating_screen)
    {
	redraw_later(type);		/* remember type for next time */
	must_redraw = type;
	if (type > INVERTED_ALL)
	    curwin->w_lines_valid = 0;	/* don't use w_lines[].wl_size now */
	return;
    }

    updating_screen = TRUE;
#ifdef FEAT_SYN_HL
    ++display_tick;	    /* let syntax code know we're in a next round of
			     * display updating */
#endif

    /*
     * if the screen was scrolled up when displaying a message, scroll it down
     */
    if (msg_scrolled)
    {
	clear_cmdline = TRUE;
	if (msg_scrolled > Rows - 5)	    /* clearing is faster */
	    type = CLEAR;
	else if (type != CLEAR)
	{
	    check_for_delay(FALSE);
	    if (screen_ins_lines(0, 0, msg_scrolled, (int)Rows, NULL) == FAIL)
		type = CLEAR;
	    FOR_ALL_WINDOWS(wp)
	    {
		if (W_WINROW(wp) < msg_scrolled)
		{
		    if (W_WINROW(wp) + wp->w_height > msg_scrolled
			    && wp->w_redr_type < REDRAW_TOP
			    && wp->w_lines_valid > 0
			    && wp->w_topline == wp->w_lines[0].wl_lnum)
		    {
			wp->w_upd_rows = msg_scrolled - W_WINROW(wp);
			wp->w_redr_type = REDRAW_TOP;
		    }
		    else
		    {
			wp->w_redr_type = NOT_VALID;
#ifdef FEAT_WINDOWS
			if (W_WINROW(wp) + wp->w_height + W_STATUS_HEIGHT(wp)
				<= msg_scrolled)
			    wp->w_redr_status = TRUE;
#endif
		    }
		}
	    }
	    redraw_cmdline = TRUE;
#ifdef FEAT_WINDOWS
	    redraw_tabline = TRUE;
#endif
	}
	msg_scrolled = 0;
	need_wait_return = FALSE;
    }

    /* reset cmdline_row now (may have been changed temporarily) */
    compute_cmdrow();

    /* Check for changed highlighting */
    if (need_highlight_changed)
	highlight_changed();

    if (type == CLEAR)		/* first clear screen */
    {
	screenclear();		/* will reset clear_cmdline */
	type = NOT_VALID;
    }

    if (clear_cmdline)		/* going to clear cmdline (done below) */
	check_for_delay(FALSE);

#ifdef FEAT_LINEBREAK
    /* Force redraw when width of 'number' or 'relativenumber' column
     * changes. */
    if (curwin->w_redr_type < NOT_VALID
	   && curwin->w_nrwidth != ((curwin->w_p_nu || curwin->w_p_rnu)
				    ? number_width(curwin) : 0))
	curwin->w_redr_type = NOT_VALID;
#endif

    /*
     * Only start redrawing if there is really something to do.
     */
    if (type == INVERTED)
	update_curswant();
    if (curwin->w_redr_type < type
	    && !((type == VALID
		    && curwin->w_lines[0].wl_valid
#ifdef FEAT_DIFF
		    && curwin->w_topfill == curwin->w_old_topfill
		    && curwin->w_botfill == curwin->w_old_botfill
#endif
		    && curwin->w_topline == curwin->w_lines[0].wl_lnum)
		|| (type == INVERTED
		    && VIsual_active
		    && curwin->w_old_cursor_lnum == curwin->w_cursor.lnum
		    && curwin->w_old_visual_mode == VIsual_mode
		    && (curwin->w_valid & VALID_VIRTCOL)
		    && curwin->w_old_curswant == curwin->w_curswant)
		))
	curwin->w_redr_type = type;

#ifdef FEAT_WINDOWS
    /* Redraw the tab pages line if needed. */
    if (redraw_tabline || type >= NOT_VALID)
	draw_tabline();
#endif

#ifdef FEAT_SYN_HL
    /*
     * Correct stored syntax highlighting info for changes in each displayed
     * buffer.  Each buffer must only be done once.
     */
    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_buffer->b_mod_set)
	{
# ifdef FEAT_WINDOWS
	    win_T	*wwp;

	    /* Check if we already did this buffer. */
	    for (wwp = firstwin; wwp != wp; wwp = wwp->w_next)
		if (wwp->w_buffer == wp->w_buffer)
		    break;
# endif
	    if (
# ifdef FEAT_WINDOWS
		    wwp == wp &&
# endif
		    syntax_present(wp))
		syn_stack_apply_changes(wp->w_buffer);
	}
    }
#endif

    /*
     * Go from top to bottom through the windows, redrawing the ones that need
     * it.
     */
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
    did_one = FALSE;
#endif
#ifdef FEAT_SEARCH_EXTRA
    search_hl.rm.regprog = NULL;
#endif
    FOR_ALL_WINDOWS(wp)
    {
	if (wp->w_redr_type != 0)
	{
	    cursor_off();
#if defined(FEAT_SEARCH_EXTRA) || defined(FEAT_CLIPBOARD)
	    if (!did_one)
	    {
		did_one = TRUE;
# ifdef FEAT_SEARCH_EXTRA
		start_search_hl();
# endif
# ifdef FEAT_CLIPBOARD
		/* When Visual area changed, may have to update selection. */
		if (clip_star.available && clip_isautosel_star())
		    clip_update_selection(&clip_star);
		if (clip_plus.available && clip_isautosel_plus())
		    clip_update_selection(&clip_plus);
# endif
#ifdef FEAT_GUI
		/* Remove the cursor before starting to do anything, because
		 * scrolling may make it difficult to redraw the text under
		 * it. */
		if (gui.in_use)
		    gui_undraw_cursor();
#endif
	    }
#endif
	    win_update(wp);
	}

#ifdef FEAT_WINDOWS
	/* redraw status line after the window to minimize cursor movement */
	if (wp->w_redr_status)
	{
	    cursor_off();
	    win_redr_status(wp);
	}
#endif
    }
#if defined(FEAT_SEARCH_EXTRA)
    end_search_hl();
#endif
#ifdef FEAT_INS_EXPAND
    /* May need to redraw the popup menu. */
    if (pum_visible())
	pum_redraw();
#endif

#ifdef FEAT_WINDOWS
    /* Reset b_mod_set flags.  Going through all windows is probably faster
     * than going through all buffers (there could be many buffers). */
    for (wp = firstwin; wp != NULL; wp = wp->w_next)
	wp->w_buffer->b_mod_set = FALSE;
#else
	curbuf->b_mod_set = FALSE;
#endif

    updating_screen = FALSE;
#ifdef FEAT_GUI
    gui_may_resize_shell();
#endif

    /* Clear or redraw the command line.  Done last, because scrolling may
     * mess up the command line. */
    if (clear_cmdline || redraw_cmdline)
	showmode();

    /* May put up an introductory message when not editing a file */
    if (!did_intro)
	maybe_intro_message();
    did_intro = TRUE;

#ifdef FEAT_GUI
    /* Redraw the cursor and update the scrollbars when all screen updating is
     * done. */
    if (gui.in_use)
    {
	out_flush();	/* required before updating the cursor */
	if (did_one)
	    gui_update_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
#endif
}

#if defined(FEAT_CONCEAL) || defined(PROTO)
/*
 * Return TRUE if the cursor line in window "wp" may be concealed, according
 * to the 'concealcursor' option.
 */
    int
conceal_cursor_line(win_T *wp)
{
    int		c;

    if (*wp->w_p_cocu == NUL)
	return FALSE;
    if (get_real_state() & VISUAL)
	c = 'v';
    else if (State & INSERT)
	c = 'i';
    else if (State & NORMAL)
	c = 'n';
    else if (State & CMDLINE)
	c = 'c';
    else
	return FALSE;
    return vim_strchr(wp->w_p_cocu, c) != NULL;
}

/*
 * Check if the cursor line needs to be redrawn because of 'concealcursor'.
 */
    void
conceal_check_cursur_line(void)
{
    if (curwin->w_p_cole > 0 && conceal_cursor_line(curwin))
    {
	need_cursor_line_redraw = TRUE;
	/* Need to recompute cursor column, e.g., when starting Visual mode
	 * without concealing. */
	curs_columns(TRUE);
    }
}

    void
update_single_line(win_T *wp, linenr_T lnum)
{
    int		row;
    int		j;

    if (lnum >= wp->w_topline && lnum < wp->w_botline
				 && foldedCount(wp, lnum, &win_foldinfo) == 0)
    {
# ifdef FEAT_GUI
	/* Remove the cursor before starting to do anything, because scrolling
	 * may make it difficult to redraw the text under it. */
	if (gui.in_use)
	    gui_undraw_cursor();
# endif
	row = 0;
	for (j = 0; j < wp->w_lines_valid; ++j)
	{
	    if (lnum == wp->w_lines[j].wl_lnum)
	    {
		screen_start();	/* not sure of screen cursor */
# ifdef FEAT_SEARCH_EXTRA
		init_search_hl(wp);
		start_search_hl();
		prepare_search_hl(wp, lnum);
# endif
		win_line(wp, lnum, row, row + wp->w_lines[j].wl_size, FALSE);
# if defined(FEAT_SEARCH_EXTRA)
		end_search_hl();
# endif
		break;
	    }
	    row += wp->w_lines[j].wl_size;
	}
# ifdef FEAT_GUI
	/* Redraw the cursor */
	if (gui.in_use)
	{
	    out_flush();	/* required before updating the cursor */
	    gui_update_cursor(FALSE, FALSE);
	}
# endif
    }
    need_cursor_line_redraw = FALSE;
}
#endif

#if defined(FEAT_SIGNS) || defined(FEAT_GUI)
static void update_prepare(void);
static void update_finish(void);

/*
 * Prepare for updating one or more windows.
 * Caller must check for "updating_screen" already set to avoid recursiveness.
 */
    static void
update_prepare(void)
{
    cursor_off();
    updating_screen = TRUE;
#ifdef FEAT_GUI
    /* Remove the cursor before starting to do anything, because scrolling may
     * make it difficult to redraw the text under it. */
    if (gui.in_use)
	gui_undraw_cursor();
#endif
#ifdef FEAT_SEARCH_EXTRA
    start_search_hl();
#endif
}

/*
 * Finish updating one or more windows.
 */
    static void
update_finish(void)
{
    if (redraw_cmdline)
	showmode();

# ifdef FEAT_SEARCH_EXTRA
    end_search_hl();
# endif

    updating_screen = FALSE;

# ifdef FEAT_GUI
    gui_may_resize_shell();

    /* Redraw the cursor and update the scrollbars when all screen updating is
     * done. */
    if (gui.in_use)
    {
	out_flush();	/* required before updating the cursor */
	gui_update_cursor(FALSE, FALSE);
	gui_update_scrollbars(FALSE);
    }
# endif
}
#endif

#if defined(FEAT_SIGNS) || defined(PROTO)
    void
update_debug_sign(buf_T *buf, linenr_T lnum)
{
    win_T	*wp;
    int		doit = FALSE;

# ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
# endif

    /* update/delete a specific mark */
    FOR_ALL_WINDOWS(wp)
    {
	if (buf != NULL && lnum > 0)
	{
	    if (wp->w_buffer == buf && lnum >= wp->w_topline
						      && lnum < wp->w_botline)
	    {
		if (wp->w_redraw_top == 0 || wp->w_redraw_top > lnum)
		    wp->w_redraw_top = lnum;
		if (wp->w_redraw_bot == 0 || wp->w_redraw_bot < lnum)
		    wp->w_redraw_bot = lnum;
		redraw_win_later(wp, VALID);
	    }
	}
	else
	    redraw_win_later(wp, VALID);
	if (wp->w_redr_type != 0)
	    doit = TRUE;
    }

    /* Return when there is nothing to do, screen updating is already
     * happening (recursive call) or still starting up. */
    if (!doit || updating_screen
#ifdef FEAT_GUI
	    || gui.starting
#endif
	    || starting)
	return;

    /* update all windows that need updating */
    update_prepare();

# ifdef FEAT_WINDOWS
    for (wp = firstwin; wp; wp = wp->w_next)
    {
	if (wp->w_redr_type != 0)
	    win_update(wp);
	if (wp->w_redr_status)
	    win_redr_status(wp);
    }
# else
    if (curwin->w_redr_type != 0)
	win_update(curwin);
# endif

    update_finish();
}
#endif


#if defined(FEAT_GUI) || defined(PROTO)
/*
 * Update a single window, its status line and maybe the command line msg.
 * Used for the GUI scrollbar.
 */
    void
updateWindow(win_T *wp)
{
    /* return if already busy updating */
    if (updating_screen)
	return;

    update_prepare();

#ifdef FEAT_CLIPBOARD
    /* When Visual area changed, may have to update selection. */
    if (clip_star.available && clip_isautosel_star())
	clip_update_selection(&clip_star);
    if (clip_plus.available && clip_isautosel_plus())
	clip_update_selection(&clip_plus);
#endif

    win_update(wp);

#ifdef FEAT_WINDOWS
    /* When the screen was cleared redraw the tab pages line. */
    if (redraw_tabline)
	draw_tabline();

    if (wp->w_redr_status
# ifdef FEAT_CMDL_INFO
	    || p_ru
# endif
# ifdef FEAT_STL_OPT
	    || *p_stl != NUL || *wp->w_p_stl != NUL
# endif
	    )
	win_redr_status(wp);
#endif

    update_finish();
}
#endif

/*
 * Update a single window.
 *
 * This may cause the windows below it also to be redrawn (when clearing the
 * screen or scrolling lines).
 *
 * How the window is redrawn depends on wp->w_redr_type.  Each type also
 * implies the one below it.
 * NOT_VALID	redraw the whole window
 * SOME_VALID	redraw the whole window but do scroll when possible
 * REDRAW_TOP	redraw the top w_upd_rows window lines, otherwise like VALID
 * INVERTED	redraw the changed part of the Visual area
 * INVERTED_ALL	redraw the whole Visual area
 * VALID	1. scroll up/down to adjust for a changed w_topline
 *		2. update lines at the top when scrolled down
 *		3. redraw changed text:
 *		   - if wp->w_buffer->b_mod_set set, update lines between
 *		     b_mod_top and b_mod_bot.
 *		   - if wp->w_redraw_top non-zero, redraw lines between
 *		     wp->w_redraw_top and wp->w_redr_bot.
 *		   - continue redrawing when syntax status is invalid.
 *		4. if scrolled up, update lines at the bottom.
 * This results in three areas that may need updating:
 * top:	from first row to top_end (when scrolled down)
 * mid: from mid_start to mid_end (update inversion or changed text)
 * bot: from bot_start to last row (when scrolled up)
 */
    static void
win_update(win_T *wp)
{
    buf_T	*buf = wp->w_buffer;
    int		type;
    int		top_end = 0;	/* Below last row of the top area that needs
				   updating.  0 when no top area updating. */
    int		mid_start = 999;/* first row of the mid area that needs
				   updating.  999 when no mid area updating. */
    int		mid_end = 0;	/* Below last row of the mid area that needs
				   updating.  0 when no mid area updating. */
    int		bot_start = 999;/* first row of the bot area that needs
				   updating.  999 when no bot area updating */
    int		scrolled_down = FALSE;	/* TRUE when scrolled down when
					   w_topline got smaller a bit */
#ifdef FEAT_SEARCH_EXTRA
    matchitem_T *cur;		/* points to the match list */
    int		top_to_mod = FALSE;    /* redraw above mod_top */
#endif

    int		row;		/* current window row to display */
    linenr_T	lnum;		/* current buffer lnum to display */
    int		idx;		/* current index in w_lines[] */
    int		srow;		/* starting row of the current line */

    int		eof = FALSE;	/* if TRUE, we hit the end of the file */
    int		didline = FALSE; /* if TRUE, we finished the last line */
    int		i;
    long	j;
    static int	recursive = FALSE;	/* being called recursively */
    int		old_botline = wp->w_botline;
#ifdef FEAT_FOLDING
    long	fold_count;
#endif
#ifdef FEAT_SYN_HL
    /* remember what happened to the previous line, to know if
     * check_visual_highlight() can be used */
#define DID_NONE 1	/* didn't update a line */
#define DID_LINE 2	/* updated a normal line */
#define DID_FOLD 3	/* updated a folded line */
    int		did_update = DID_NONE;
    linenr_T	syntax_last_parsed = 0;		/* last parsed text line */
#endif
    linenr_T	mod_top = 0;
    linenr_T	mod_bot = 0;
#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    int		save_got_int;
#endif

    type = wp->w_redr_type;

    if (type == NOT_VALID)
    {
#ifdef FEAT_WINDOWS
	wp->w_redr_status = TRUE;
#endif
	wp->w_lines_valid = 0;
    }

    /* Window is zero-height: nothing to draw. */
    if (wp->w_height == 0)
    {
	wp->w_redr_type = 0;
	return;
    }

#ifdef FEAT_WINDOWS
    /* Window is zero-width: Only need to draw the separator. */
    if (wp->w_width == 0)
    {
	/* draw the vertical separator right of this window */
	draw_vsep_win(wp, 0);
	wp->w_redr_type = 0;
	return;
    }
#endif

#ifdef FEAT_SEARCH_EXTRA
    init_search_hl(wp);
#endif

#ifdef FEAT_LINEBREAK
    /* Force redraw when width of 'number' or 'relativenumber' column
     * changes. */
    i = (wp->w_p_nu || wp->w_p_rnu) ? number_width(wp) : 0;
    if (wp->w_nrwidth != i)
    {
	type = NOT_VALID;
	wp->w_nrwidth = i;
    }
    else
#endif

    if (buf->b_mod_set && buf->b_mod_xlines != 0 && wp->w_redraw_top != 0)
    {
	/*
	 * When there are both inserted/deleted lines and specific lines to be
	 * redrawn, w_redraw_top and w_redraw_bot may be invalid, just redraw
	 * everything (only happens when redrawing is off for while).
	 */
	type = NOT_VALID;
    }
    else
    {
	/*
	 * Set mod_top to the first line that needs displaying because of
	 * changes.  Set mod_bot to the first line after the changes.
	 */
	mod_top = wp->w_redraw_top;
	if (wp->w_redraw_bot != 0)
	    mod_bot = wp->w_redraw_bot + 1;
	else
	    mod_bot = 0;
	wp->w_redraw_top = 0;	/* reset for next time */
	wp->w_redraw_bot = 0;
	if (buf->b_mod_set)
	{
	    if (mod_top == 0 || mod_top > buf->b_mod_top)
	    {
		mod_top = buf->b_mod_top;
#ifdef FEAT_SYN_HL
		/* Need to redraw lines above the change that may be included
		 * in a pattern match. */
		if (syntax_present(wp))
		{
		    mod_top -= buf->b_s.b_syn_sync_linebreaks;
		    if (mod_top < 1)
			mod_top = 1;
		}
#endif
	    }
	    if (mod_bot == 0 || mod_bot < buf->b_mod_bot)
		mod_bot = buf->b_mod_bot;

#ifdef FEAT_SEARCH_EXTRA
	    /* When 'hlsearch' is on and using a multi-line search pattern, a
	     * change in one line may make the Search highlighting in a
	     * previous line invalid.  Simple solution: redraw all visible
	     * lines above the change.
	     * Same for a match pattern.
	     */
	    if (search_hl.rm.regprog != NULL
					&& re_multiline(search_hl.rm.regprog))
		top_to_mod = TRUE;
	    else
	    {
		cur = wp->w_match_head;
		while (cur != NULL)
		{
		    if (cur->match.regprog != NULL
					   && re_multiline(cur->match.regprog))
		    {
			top_to_mod = TRUE;
			break;
		    }
		    cur = cur->next;
		}
	    }
#endif
	}
#ifdef FEAT_FOLDING
	if (mod_top != 0 && hasAnyFolding(wp))
	{
	    linenr_T	lnumt, lnumb;

	    /*
	     * A change in a line can cause lines above it to become folded or
	     * unfolded.  Find the top most buffer line that may be affected.
	     * If the line was previously folded and displayed, get the first
	     * line of that fold.  If the line is folded now, get the first
	     * folded line.  Use the minimum of these two.
	     */

	    /* Find last valid w_lines[] entry above mod_top.  Set lnumt to
	     * the line below it.  If there is no valid entry, use w_topline.
	     * Find the first valid w_lines[] entry below mod_bot.  Set lnumb
	     * to this line.  If there is no valid entry, use MAXLNUM. */
	    lnumt = wp->w_topline;
	    lnumb = MAXLNUM;
	    for (i = 0; i < wp->w_lines_valid; ++i)
		if (wp->w_lines[i].wl_valid)
		{
		    if (wp->w_lines[i].wl_lastlnum < mod_top)
			lnumt = wp->w_lines[i].wl_lastlnum + 1;
		    if (lnumb == MAXLNUM && wp->w_lines[i].wl_lnum >= mod_bot)
		    {
			lnumb = wp->w_lines[i].wl_lnum;
			/* When there is a fold column it might need updating
			 * in the next line ("J" just above an open fold). */
			if (compute_foldcolumn(wp, 0) > 0)
			    ++lnumb;
		    }
		}

	    (void)hasFoldingWin(wp, mod_top, &mod_top, NULL, TRUE, NULL);
	    if (mod_top > lnumt)
		mod_top = lnumt;

	    /* Now do the same for the bottom line (one above mod_bot). */
	    --mod_bot;
	    (void)hasFoldingWin(wp, mod_bot, NULL, &mod_bot, TRUE, NULL);
	    ++mod_bot;
	    if (mod_bot < lnumb)
		mod_bot = lnumb;
	}
#endif

	/* When a change starts above w_topline and the end is below
	 * w_topline, start redrawing at w_topline.
	 * If the end of the change is above w_topline: do like no change was
	 * made, but redraw the first line to find changes in syntax. */
	if (mod_top != 0 && mod_top < wp->w_topline)
	{
	    if (mod_bot > wp->w_topline)
		mod_top = wp->w_topline;
#ifdef FEAT_SYN_HL
	    else if (syntax_present(wp))
		top_end = 1;
#endif
	}

	/* When line numbers are displayed need to redraw all lines below
	 * inserted/deleted lines. */
	if (mod_top != 0 && buf->b_mod_xlines != 0 && wp->w_p_nu)
	    mod_bot = MAXLNUM;
    }

    /*
     * When only displaying the lines at the top, set top_end.  Used when
     * window has scrolled down for msg_scrolled.
     */
    if (type == REDRAW_TOP)
    {
	j = 0;
	for (i = 0; i < wp->w_lines_valid; ++i)
	{
	    j += wp->w_lines[i].wl_size;
	    if (j >= wp->w_upd_rows)
	    {
		top_end = j;
		break;
	    }
	}
	if (top_end == 0)
	    /* not found (cannot happen?): redraw everything */
	    type = NOT_VALID;
	else
	    /* top area defined, the rest is VALID */
	    type = VALID;
    }

    /* Trick: we want to avoid clearing the screen twice.  screenclear() will
     * set "screen_cleared" to TRUE.  The special value MAYBE (which is still
     * non-zero and thus not FALSE) will indicate that screenclear() was not
     * called. */
    if (screen_cleared)
	screen_cleared = MAYBE;

    /*
     * If there are no changes on the screen that require a complete redraw,
     * handle three cases:
     * 1: we are off the top of the screen by a few lines: scroll down
     * 2: wp->w_topline is below wp->w_lines[0].wl_lnum: may scroll up
     * 3: wp->w_topline is wp->w_lines[0].wl_lnum: find first entry in
     *    w_lines[] that needs updating.
     */
    if ((type == VALID || type == SOME_VALID
				  || type == INVERTED || type == INVERTED_ALL)
#ifdef FEAT_DIFF
	    && !wp->w_botfill && !wp->w_old_botfill
#endif
	    )
    {
	if (mod_top != 0 && wp->w_topline == mod_top)
	{
	    /*
	     * w_topline is the first changed line, the scrolling will be done
	     * further down.
	     */
	}
	else if (wp->w_lines[0].wl_valid
		&& (wp->w_topline < wp->w_lines[0].wl_lnum
#ifdef FEAT_DIFF
		    || (wp->w_topline == wp->w_lines[0].wl_lnum
			&& wp->w_topfill > wp->w_old_topfill)
#endif
		   ))
	{
	    /*
	     * New topline is above old topline: May scroll down.
	     */
#ifdef FEAT_FOLDING
	    if (hasAnyFolding(wp))
	    {
		linenr_T ln;

		/* count the number of lines we are off, counting a sequence
		 * of folded lines as one */
		j = 0;
		for (ln = wp->w_topline; ln < wp->w_lines[0].wl_lnum; ++ln)
		{
		    ++j;
		    if (j >= wp->w_height - 2)
			break;
		    (void)hasFoldingWin(wp, ln, NULL, &ln, TRUE, NULL);
		}
	    }
	    else
#endif
		j = wp->w_lines[0].wl_lnum - wp->w_topline;
	    if (j < wp->w_height - 2)		/* not too far off */
	    {
		i = plines_m_win(wp, wp->w_topline, wp->w_lines[0].wl_lnum - 1);
#ifdef FEAT_DIFF
		/* insert extra lines for previously invisible filler lines */
		if (wp->w_lines[0].wl_lnum != wp->w_topline)
		    i += diff_check_fill(wp, wp->w_lines[0].wl_lnum)
							  - wp->w_old_topfill;
#endif
		if (i < wp->w_height - 2)	/* less than a screen off */
		{
		    /*
		     * Try to insert the correct number of lines.
		     * If not the last window, delete the lines at the bottom.
		     * win_ins_lines may fail when the terminal can't do it.
		     */
		    if (i > 0)
			check_for_delay(FALSE);
		    if (win_ins_lines(wp, 0, i, FALSE, wp == firstwin) == OK)
		    {
			if (wp->w_lines_valid != 0)
			{
			    /* Need to update rows that are new, stop at the
			     * first one that scrolled down. */
			    top_end = i;
			    scrolled_down = TRUE;

			    /* Move the entries that were scrolled, disable
			     * the entries for the lines to be redrawn. */
			    if ((wp->w_lines_valid += j) > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    for (idx = wp->w_lines_valid; idx - j >= 0; idx--)
				wp->w_lines[idx] = wp->w_lines[idx - j];
			    while (idx >= 0)
				wp->w_lines[idx--].wl_valid = FALSE;
			}
		    }
		    else
			mid_start = 0;		/* redraw all lines */
		}
		else
		    mid_start = 0;		/* redraw all lines */
	    }
	    else
		mid_start = 0;		/* redraw all lines */
	}
	else
	{
	    /*
	     * New topline is at or below old topline: May scroll up.
	     * When topline didn't change, find first entry in w_lines[] that
	     * needs updating.
	     */

	    /* try to find wp->w_topline in wp->w_lines[].wl_lnum */
	    j = -1;
	    row = 0;
	    for (i = 0; i < wp->w_lines_valid; i++)
	    {
		if (wp->w_lines[i].wl_valid
			&& wp->w_lines[i].wl_lnum == wp->w_topline)
		{
		    j = i;
		    break;
		}
		row += wp->w_lines[i].wl_size;
	    }
	    if (j == -1)
	    {
		/* if wp->w_topline is not in wp->w_lines[].wl_lnum redraw all
		 * lines */
		mid_start = 0;
	    }
	    else
	    {
		/*
		 * Try to delete the correct number of lines.
		 * wp->w_topline is at wp->w_lines[i].wl_lnum.
		 */
#ifdef FEAT_DIFF
		/* If the topline didn't change, delete old filler lines,
		 * otherwise delete filler lines of the new topline... */
		if (wp->w_lines[0].wl_lnum == wp->w_topline)
		    row += wp->w_old_topfill;
		else
		    row += diff_check_fill(wp, wp->w_topline);
		/* ... but don't delete new filler lines. */
		row -= wp->w_topfill;
#endif
		if (row > 0)
		{
		    check_for_delay(FALSE);
		    if (win_del_lines(wp, 0, row, FALSE, wp == firstwin) == OK)
			bot_start = wp->w_height - row;
		    else
			mid_start = 0;		/* redraw all lines */
		}
		if ((row == 0 || bot_start < 999) && wp->w_lines_valid != 0)
		{
		    /*
		     * Skip the lines (below the deleted lines) that are still
		     * valid and don't need redrawing.	Copy their info
		     * upwards, to compensate for the deleted lines.  Set
		     * bot_start to the first row that needs redrawing.
		     */
		    bot_start = 0;
		    idx = 0;
		    for (;;)
		    {
			wp->w_lines[idx] = wp->w_lines[j];
			/* stop at line that didn't fit, unless it is still
			 * valid (no lines deleted) */
			if (row > 0 && bot_start + row
				 + (int)wp->w_lines[j].wl_size > wp->w_height)
			{
			    wp->w_lines_valid = idx + 1;
			    break;
			}
			bot_start += wp->w_lines[idx++].wl_size;

			/* stop at the last valid entry in w_lines[].wl_size */
			if (++j >= wp->w_lines_valid)
			{
			    wp->w_lines_valid = idx;
			    break;
			}
		    }
#ifdef FEAT_DIFF
		    /* Correct the first entry for filler lines at the top
		     * when it won't get updated below. */
		    if (wp->w_p_diff && bot_start > 0)
			wp->w_lines[0].wl_size =
			    plines_win_nofill(wp, wp->w_topline, TRUE)
							      + wp->w_topfill;
#endif
		}
	    }
	}

	/* When starting redraw in the first line, redraw all lines.  When
	 * there is only one window it's probably faster to clear the screen
	 * first. */
	if (mid_start == 0)
	{
	    mid_end = wp->w_height;
	    if (lastwin == firstwin)
	    {
		/* Clear the screen when it was not done by win_del_lines() or
		 * win_ins_lines() above, "screen_cleared" is FALSE or MAYBE
		 * then. */
		if (screen_cleared != TRUE)
		    screenclear();
#ifdef FEAT_WINDOWS
		/* The screen was cleared, redraw the tab pages line. */
		if (redraw_tabline)
		    draw_tabline();
#endif
	    }
	}

	/* When win_del_lines() or win_ins_lines() caused the screen to be
	 * cleared (only happens for the first window) or when screenclear()
	 * was called directly above, "must_redraw" will have been set to
	 * NOT_VALID, need to reset it here to avoid redrawing twice. */
	if (screen_cleared == TRUE)
	    must_redraw = 0;
    }
    else
    {
	/* Not VALID or INVERTED: redraw all lines. */
	mid_start = 0;
	mid_end = wp->w_height;
    }

    if (type == SOME_VALID)
    {
	/* SOME_VALID: redraw all lines. */
	mid_start = 0;
	mid_end = wp->w_height;
	type = NOT_VALID;
    }

    /* check if we are updating or removing the inverted part */
    if ((VIsual_active && buf == curwin->w_buffer)
	    || (wp->w_old_cursor_lnum != 0 && type != NOT_VALID))
    {
	linenr_T    from, to;

	if (VIsual_active)
	{
	    if (VIsual_active
		    && (VIsual_mode != wp->w_old_visual_mode
			|| type == INVERTED_ALL))
	    {
		/*
		 * If the type of Visual selection changed, redraw the whole
		 * selection.  Also when the ownership of the X selection is
		 * gained or lost.
		 */
		if (curwin->w_cursor.lnum < VIsual.lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = VIsual.lnum;
		}
		else
		{
		    from = VIsual.lnum;
		    to = curwin->w_cursor.lnum;
		}
		/* redraw more when the cursor moved as well */
		if (wp->w_old_cursor_lnum < from)
		    from = wp->w_old_cursor_lnum;
		if (wp->w_old_cursor_lnum > to)
		    to = wp->w_old_cursor_lnum;
		if (wp->w_old_visual_lnum < from)
		    from = wp->w_old_visual_lnum;
		if (wp->w_old_visual_lnum > to)
		    to = wp->w_old_visual_lnum;
	    }
	    else
	    {
		/*
		 * Find the line numbers that need to be updated: The lines
		 * between the old cursor position and the current cursor
		 * position.  Also check if the Visual position changed.
		 */
		if (curwin->w_cursor.lnum < wp->w_old_cursor_lnum)
		{
		    from = curwin->w_cursor.lnum;
		    to = wp->w_old_cursor_lnum;
		}
		else
		{
		    from = wp->w_old_cursor_lnum;
		    to = curwin->w_cursor.lnum;
		    if (from == 0)	/* Visual mode just started */
			from = to;
		}

		if (VIsual.lnum != wp->w_old_visual_lnum
					|| VIsual.col != wp->w_old_visual_col)
		{
		    if (wp->w_old_visual_lnum < from
						&& wp->w_old_visual_lnum != 0)
			from = wp->w_old_visual_lnum;
		    if (wp->w_old_visual_lnum > to)
			to = wp->w_old_visual_lnum;
		    if (VIsual.lnum < from)
			from = VIsual.lnum;
		    if (VIsual.lnum > to)
			to = VIsual.lnum;
		}
	    }

	    /*
	     * If in block mode and changed column or curwin->w_curswant:
	     * update all lines.
	     * First compute the actual start and end column.
	     */
	    if (VIsual_mode == Ctrl_V)
	    {
		colnr_T	    fromc, toc;
#if defined(FEAT_VIRTUALEDIT) && defined(FEAT_LINEBREAK)
		int	    save_ve_flags = ve_flags;

		if (curwin->w_p_lbr)
		    ve_flags = VE_ALL;
#endif
		getvcols(wp, &VIsual, &curwin->w_cursor, &fromc, &toc);
#if defined(FEAT_VIRTUALEDIT) && defined(FEAT_LINEBREAK)
		ve_flags = save_ve_flags;
#endif
		++toc;
		if (curwin->w_curswant == MAXCOL)
		    toc = MAXCOL;

		if (fromc != wp->w_old_cursor_fcol
			|| toc != wp->w_old_cursor_lcol)
		{
		    if (from > VIsual.lnum)
			from = VIsual.lnum;
		    if (to < VIsual.lnum)
			to = VIsual.lnum;
		}
		wp->w_old_cursor_fcol = fromc;
		wp->w_old_cursor_lcol = toc;
	    }
	}
	else
	{
	    /* Use the line numbers of the old Visual area. */
	    if (wp->w_old_cursor_lnum < wp->w_old_visual_lnum)
	    {
		from = wp->w_old_cursor_lnum;
		to = wp->w_old_visual_lnum;
	    }
	    else
	    {
		from = wp->w_old_visual_lnum;
		to = wp->w_old_cursor_lnum;
	    }
	}

	/*
	 * There is no need to update lines above the top of the window.
	 */
	if (from < wp->w_topline)
	    from = wp->w_topline;

	/*
	 * If we know the value of w_botline, use it to restrict the update to
	 * the lines that are visible in the window.
	 */
	if (wp->w_valid & VALID_BOTLINE)
	{
	    if (from >= wp->w_botline)
		from = wp->w_botline - 1;
	    if (to >= wp->w_botline)
		to = wp->w_botline - 1;
	}

	/*
	 * Find the minimal part to be updated.
	 * Watch out for scrolling that made entries in w_lines[] invalid.
	 * E.g., CTRL-U makes the first half of w_lines[] invalid and sets
	 * top_end; need to redraw from top_end to the "to" line.
	 * A middle mouse click with a Visual selection may change the text
	 * above the Visual area and reset wl_valid, do count these for
	 * mid_end (in srow).
	 */
	if (mid_start > 0)
	{
	    lnum = wp->w_topline;
	    idx = 0;
	    srow = 0;
	    if (scrolled_down)
		mid_start = top_end;
	    else
		mid_start = 0;
	    while (lnum < from && idx < wp->w_lines_valid)	/* find start */
	    {
		if (wp->w_lines[idx].wl_valid)
		    mid_start += wp->w_lines[idx].wl_size;
		else if (!scrolled_down)
		    srow += wp->w_lines[idx].wl_size;
		++idx;
# ifdef FEAT_FOLDING
		if (idx < wp->w_lines_valid && wp->w_lines[idx].wl_valid)
		    lnum = wp->w_lines[idx].wl_lnum;
		else
# endif
		    ++lnum;
	    }
	    srow += mid_start;
	    mid_end = wp->w_height;
	    for ( ; idx < wp->w_lines_valid; ++idx)		/* find end */
	    {
		if (wp->w_lines[idx].wl_valid
			&& wp->w_lines[idx].wl_lnum >= to + 1)
		{
		    /* Only update until first row of this line */
		    mid_end = srow;
		    break;
		}
		srow += wp->w_lines[idx].wl_size;
	    }
	}
    }

    if (VIsual_active && buf == curwin->w_buffer)
    {
	wp->w_old_visual_mode = VIsual_mode;
	wp->w_old_cursor_lnum = curwin->w_cursor.lnum;
	wp->w_old_visual_lnum = VIsual.lnum;
	wp->w_old_visual_col = VIsual.col;
	wp->w_old_curswant = curwin->w_curswant;
    }
    else
    {
	wp->w_old_visual_mode = 0;
	wp->w_old_cursor_lnum = 0;
	wp->w_old_visual_lnum = 0;
	wp->w_old_visual_col = 0;
    }

#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    /* reset got_int, otherwise regexp won't work */
    save_got_int = got_int;
    got_int = 0;
#endif
#ifdef FEAT_FOLDING
    win_foldinfo.fi_level = 0;
#endif

    /*
     * Update all the window rows.
     */
    idx = 0;		/* first entry in w_lines[].wl_size */
    row = 0;
    srow = 0;
    lnum = wp->w_topline;	/* first line shown in window */
    for (;;)
    {
	/* stop updating when reached the end of the window (check for _past_
	 * the end of the window is at the end of the loop) */
	if (row == wp->w_height)
	{
	    didline = TRUE;
	    break;
	}

	/* stop updating when hit the end of the file */
	if (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    break;
	}

	/* Remember the starting row of the line that is going to be dealt
	 * with.  It is used further down when the line doesn't fit. */
	srow = row;

	/*
	 * Update a line when it is in an area that needs updating, when it
	 * has changes or w_lines[idx] is invalid.
	 * bot_start may be halfway a wrapped line after using
	 * win_del_lines(), check if the current line includes it.
	 * When syntax folding is being used, the saved syntax states will
	 * already have been updated, we can't see where the syntax state is
	 * the same again, just update until the end of the window.
	 */
	if (row < top_end
		|| (row >= mid_start && row < mid_end)
#ifdef FEAT_SEARCH_EXTRA
		|| top_to_mod
#endif
		|| idx >= wp->w_lines_valid
		|| (row + wp->w_lines[idx].wl_size > bot_start)
		|| (mod_top != 0
		    && (lnum == mod_top
			|| (lnum >= mod_top
			    && (lnum < mod_bot
#ifdef FEAT_SYN_HL
				|| did_update == DID_FOLD
				|| (did_update == DID_LINE
				    && syntax_present(wp)
				    && (
# ifdef FEAT_FOLDING
					(foldmethodIsSyntax(wp)
						      && hasAnyFolding(wp)) ||
# endif
					syntax_check_changed(lnum)))
#endif
#ifdef FEAT_SEARCH_EXTRA
				/* match in fixed position might need redraw
				 * if lines were inserted or deleted */
				|| (wp->w_match_head != NULL
						    && buf->b_mod_xlines != 0)
#endif
				)))))
	{
#ifdef FEAT_SEARCH_EXTRA
	    if (lnum == mod_top)
		top_to_mod = FALSE;
#endif

	    /*
	     * When at start of changed lines: May scroll following lines
	     * up or down to minimize redrawing.
	     * Don't do this when the change continues until the end.
	     * Don't scroll when dollar_vcol >= 0, keep the "$".
	     */
	    if (lnum == mod_top
		    && mod_bot != MAXLNUM
		    && !(dollar_vcol >= 0 && mod_bot == mod_top + 1))
	    {
		int		old_rows = 0;
		int		new_rows = 0;
		int		xtra_rows;
		linenr_T	l;

		/* Count the old number of window rows, using w_lines[], which
		 * should still contain the sizes for the lines as they are
		 * currently displayed. */
		for (i = idx; i < wp->w_lines_valid; ++i)
		{
		    /* Only valid lines have a meaningful wl_lnum.  Invalid
		     * lines are part of the changed area. */
		    if (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lnum == mod_bot)
			break;
		    old_rows += wp->w_lines[i].wl_size;
#ifdef FEAT_FOLDING
		    if (wp->w_lines[i].wl_valid
			    && wp->w_lines[i].wl_lastlnum + 1 == mod_bot)
		    {
			/* Must have found the last valid entry above mod_bot.
			 * Add following invalid entries. */
			++i;
			while (i < wp->w_lines_valid
						  && !wp->w_lines[i].wl_valid)
			    old_rows += wp->w_lines[i++].wl_size;
			break;
		    }
#endif
		}

		if (i >= wp->w_lines_valid)
		{
		    /* We can't find a valid line below the changed lines,
		     * need to redraw until the end of the window.
		     * Inserting/deleting lines has no use. */
		    bot_start = 0;
		}
		else
		{
		    /* Able to count old number of rows: Count new window
		     * rows, and may insert/delete lines */
		    j = idx;
		    for (l = lnum; l < mod_bot; ++l)
		    {
#ifdef FEAT_FOLDING
			if (hasFoldingWin(wp, l, NULL, &l, TRUE, NULL))
			    ++new_rows;
			else
#endif
#ifdef FEAT_DIFF
			    if (l == wp->w_topline)
			    new_rows += plines_win_nofill(wp, l, TRUE)
							      + wp->w_topfill;
			else
#endif
			    new_rows += plines_win(wp, l, TRUE);
			++j;
			if (new_rows > wp->w_height - row - 2)
			{
			    /* it's getting too much, must redraw the rest */
			    new_rows = 9999;
			    break;
			}
		    }
		    xtra_rows = new_rows - old_rows;
		    if (xtra_rows < 0)
		    {
			/* May scroll text up.  If there is not enough
			 * remaining text or scrolling fails, must redraw the
			 * rest.  If scrolling works, must redraw the text
			 * below the scrolled text. */
			if (row - xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			else
			{
			    check_for_delay(FALSE);
			    if (win_del_lines(wp, row,
					    -xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    else
				bot_start = wp->w_height + xtra_rows;
			}
		    }
		    else if (xtra_rows > 0)
		    {
			/* May scroll text down.  If there is not enough
			 * remaining text of scrolling fails, must redraw the
			 * rest. */
			if (row + xtra_rows >= wp->w_height - 2)
			    mod_bot = MAXLNUM;
			else
			{
			    check_for_delay(FALSE);
			    if (win_ins_lines(wp, row + old_rows,
					     xtra_rows, FALSE, FALSE) == FAIL)
				mod_bot = MAXLNUM;
			    else if (top_end > row + old_rows)
				/* Scrolled the part at the top that requires
				 * updating down. */
				top_end += xtra_rows;
			}
		    }

		    /* When not updating the rest, may need to move w_lines[]
		     * entries. */
		    if (mod_bot != MAXLNUM && i != j)
		    {
			if (j < i)
			{
			    int x = row + new_rows;

			    /* move entries in w_lines[] upwards */
			    for (;;)
			    {
				/* stop at last valid entry in w_lines[] */
				if (i >= wp->w_lines_valid)
				{
				    wp->w_lines_valid = j;
				    break;
				}
				wp->w_lines[j] = wp->w_lines[i];
				/* stop at a line that won't fit */
				if (x + (int)wp->w_lines[j].wl_size
							   > wp->w_height)
				{
				    wp->w_lines_valid = j + 1;
				    break;
				}
				x += wp->w_lines[j++].wl_size;
				++i;
			    }
			    if (bot_start > x)
				bot_start = x;
			}
			else /* j > i */
			{
			    /* move entries in w_lines[] downwards */
			    j -= i;
			    wp->w_lines_valid += j;
			    if (wp->w_lines_valid > wp->w_height)
				wp->w_lines_valid = wp->w_height;
			    for (i = wp->w_lines_valid; i - j >= idx; --i)
				wp->w_lines[i] = wp->w_lines[i - j];

			    /* The w_lines[] entries for inserted lines are
			     * now invalid, but wl_size may be used above.
			     * Reset to zero. */
			    while (i >= idx)
			    {
				wp->w_lines[i].wl_size = 0;
				wp->w_lines[i--].wl_valid = FALSE;
			    }
			}
		    }
		}
	    }

#ifdef FEAT_FOLDING
	    /*
	     * When lines are folded, display one line for all of them.
	     * Otherwise, display normally (can be several display lines when
	     * 'wrap' is on).
	     */
	    fold_count = foldedCount(wp, lnum, &win_foldinfo);
	    if (fold_count != 0)
	    {
		fold_line(wp, fold_count, &win_foldinfo, lnum, row);
		++row;
		--fold_count;
		wp->w_lines[idx].wl_folded = TRUE;
		wp->w_lines[idx].wl_lastlnum = lnum + fold_count;
# ifdef FEAT_SYN_HL
		did_update = DID_FOLD;
# endif
	    }
	    else
#endif
	    if (idx < wp->w_lines_valid
		    && wp->w_lines[idx].wl_valid
		    && wp->w_lines[idx].wl_lnum == lnum
		    && lnum > wp->w_topline
		    && !(dy_flags & DY_LASTLINE)
		    && srow + wp->w_lines[idx].wl_size > wp->w_height
#ifdef FEAT_DIFF
		    && diff_check_fill(wp, lnum) == 0
#endif
		    )
	    {
		/* This line is not going to fit.  Don't draw anything here,
		 * will draw "@  " lines below. */
		row = wp->w_height + 1;
	    }
	    else
	    {
#ifdef FEAT_SEARCH_EXTRA
		prepare_search_hl(wp, lnum);
#endif
#ifdef FEAT_SYN_HL
		/* Let the syntax stuff know we skipped a few lines. */
		if (syntax_last_parsed != 0 && syntax_last_parsed + 1 < lnum
						       && syntax_present(wp))
		    syntax_end_parsing(syntax_last_parsed + 1);
#endif

		/*
		 * Display one line.
		 */
		row = win_line(wp, lnum, srow, wp->w_height, mod_top == 0);

#ifdef FEAT_FOLDING
		wp->w_lines[idx].wl_folded = FALSE;
		wp->w_lines[idx].wl_lastlnum = lnum;
#endif
#ifdef FEAT_SYN_HL
		did_update = DID_LINE;
		syntax_last_parsed = lnum;
#endif
	    }

	    wp->w_lines[idx].wl_lnum = lnum;
	    wp->w_lines[idx].wl_valid = TRUE;
	    if (row > wp->w_height)	/* past end of screen */
	    {
		/* we may need the size of that too long line later on */
		if (dollar_vcol == -1)
		    wp->w_lines[idx].wl_size = plines_win(wp, lnum, TRUE);
		++idx;
		break;
	    }
	    if (dollar_vcol == -1)
		wp->w_lines[idx].wl_size = row - srow;
	    ++idx;
#ifdef FEAT_FOLDING
	    lnum += fold_count + 1;
#else
	    ++lnum;
#endif
	}
	else
	{
	    /* This line does not need updating, advance to the next one */
	    row += wp->w_lines[idx++].wl_size;
	    if (row > wp->w_height)	/* past end of screen */
		break;
#ifdef FEAT_FOLDING
	    lnum = wp->w_lines[idx - 1].wl_lastlnum + 1;
#else
	    ++lnum;
#endif
#ifdef FEAT_SYN_HL
	    did_update = DID_NONE;
#endif
	}

	if (lnum > buf->b_ml.ml_line_count)
	{
	    eof = TRUE;
	    break;
	}
    }
    /*
     * End of loop over all window lines.
     */


    if (idx > wp->w_lines_valid)
	wp->w_lines_valid = idx;

#ifdef FEAT_SYN_HL
    /*
     * Let the syntax stuff know we stop parsing here.
     */
    if (syntax_last_parsed != 0 && syntax_present(wp))
	syntax_end_parsing(syntax_last_parsed + 1);
#endif

    /*
     * If we didn't hit the end of the file, and we didn't finish the last
     * line we were working on, then the line didn't fit.
     */
    wp->w_empty_rows = 0;
#ifdef FEAT_DIFF
    wp->w_filler_rows = 0;
#endif
    if (!eof && !didline)
    {
	if (lnum == wp->w_topline)
	{
	    /*
	     * Single line that does not fit!
	     * Don't overwrite it, it can be edited.
	     */
	    wp->w_botline = lnum + 1;
	}
#ifdef FEAT_DIFF
	else if (diff_check_fill(wp, lnum) >= wp->w_height - srow)
	{
	    /* Window ends in filler lines. */
	    wp->w_botline = lnum;
	    wp->w_filler_rows = wp->w_height - srow;
	}
#endif
	else if (dy_flags & DY_LASTLINE)	/* 'display' has "lastline" */
	{
	    /*
	     * Last line isn't finished: Display "@@@" at the end.
	     */
	    screen_fill(W_WINROW(wp) + wp->w_height - 1,
		    W_WINROW(wp) + wp->w_height,
		    (int)W_ENDCOL(wp) - 3, (int)W_ENDCOL(wp),
		    '@', '@', hl_attr(HLF_AT));
	    set_empty_rows(wp, srow);
	    wp->w_botline = lnum;
	}
	else
	{
	    win_draw_end(wp, '@', ' ', srow, wp->w_height, HLF_AT);
	    wp->w_botline = lnum;
	}
    }
    else
    {
#ifdef FEAT_WINDOWS
	draw_vsep_win(wp, row);
#endif
	if (eof)		/* we hit the end of the file */
	{
	    wp->w_botline = buf->b_ml.ml_line_count + 1;
#ifdef FEAT_DIFF
	    j = diff_check_fill(wp, wp->w_botline);
	    if (j > 0 && !wp->w_botfill)
	    {
		/*
		 * Display filler lines at the end of the file
		 */
		if (char2cells(fill_diff) > 1)
		    i = '-';
		else
		    i = fill_diff;
		if (row + j > wp->w_height)
		    j = wp->w_height - row;
		win_draw_end(wp, i, i, row, row + (int)j, HLF_DED);
		row += j;
	    }
#endif
	}
	else if (dollar_vcol == -1)
	    wp->w_botline = lnum;

	/* make sure the rest of the screen is blank */
	/* put '~'s on rows that aren't part of the file. */
	win_draw_end(wp, '~', ' ', row, wp->w_height, HLF_AT);
    }

    /* Reset the type of redrawing required, the window has been updated. */
    wp->w_redr_type = 0;
#ifdef FEAT_DIFF
    wp->w_old_topfill = wp->w_topfill;
    wp->w_old_botfill = wp->w_botfill;
#endif

    if (dollar_vcol == -1)
    {
	/*
	 * There is a trick with w_botline.  If we invalidate it on each
	 * change that might modify it, this will cause a lot of expensive
	 * calls to plines() in update_topline() each time.  Therefore the
	 * value of w_botline is often approximated, and this value is used to
	 * compute the value of w_topline.  If the value of w_botline was
	 * wrong, check that the value of w_topline is correct (cursor is on
	 * the visible part of the text).  If it's not, we need to redraw
	 * again.  Mostly this just means scrolling up a few lines, so it
	 * doesn't look too bad.  Only do this for the current window (where
	 * changes are relevant).
	 */
	wp->w_valid |= VALID_BOTLINE;
	if (wp == curwin && wp->w_botline != old_botline && !recursive)
	{
	    recursive = TRUE;
	    curwin->w_valid &= ~VALID_TOPLINE;
	    update_topline();	/* may invalidate w_botline again */
	    if (must_redraw != 0)
	    {
		/* Don't update for changes in buffer again. */
		i = curbuf->b_mod_set;
		curbuf->b_mod_set = FALSE;
		win_update(curwin);
		must_redraw = 0;
		curbuf->b_mod_set = i;
	    }
	    recursive = FALSE;
	}
    }

#if defined(FEAT_SYN_HL) || defined(FEAT_SEARCH_EXTRA)
    /* restore got_int, unless CTRL-C was hit while redrawing */
    if (!got_int)
	got_int = save_got_int;
#endif
}

#ifdef FEAT_SIGNS
static int draw_signcolumn(win_T *wp);

/*
 * Return TRUE when window "wp" has a column to draw signs in.
 */
    static int
draw_signcolumn(win_T *wp)
{
    return (wp->w_buffer->b_signlist != NULL
# ifdef FEAT_NETBEANS_INTG
				|| wp->w_buffer->b_has_sign_column
# endif
		    );
}
#endif

/*
 * Clear the rest of the window and mark the unused lines with "c1".  use "c2"
 * as the filler character.
 */
    static void
win_draw_end(
    win_T	*wp,
    int		c1,
    int		c2,
    int		row,
    int		endrow,
    hlf_T	hl)
{
#if defined(FEAT_FOLDING) || defined(FEAT_SIGNS) || defined(FEAT_CMDWIN)
    int		n = 0;
# define FDC_OFF n
#else
# define FDC_OFF 0
#endif
#ifdef FEAT_FOLDING
    int		fdc = compute_foldcolumn(wp, 0);
#endif

#ifdef FEAT_RIGHTLEFT
    if (wp->w_p_rl)
    {
	/* No check for cmdline window: should never be right-left. */
# ifdef FEAT_FOLDING
	n = fdc;

	if (n > 0)
	{
	    /* draw the fold column at the right */
	    if (n > W_WIDTH(wp))
		n = W_WIDTH(wp);
	    screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		    W_ENDCOL(wp) - n, (int)W_ENDCOL(wp),
		    ' ', ' ', hl_attr(HLF_FC));
	}
# endif
# ifdef FEAT_SIGNS
	if (draw_signcolumn(wp))
	{
	    int nn = n + 2;

	    /* draw the sign column left of the fold column */
	    if (nn > W_WIDTH(wp))
		nn = W_WIDTH(wp);
	    screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		    W_ENDCOL(wp) - nn, (int)W_ENDCOL(wp) - n,
		    ' ', ' ', hl_attr(HLF_SC));
	    n = nn;
	}
# endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_WINCOL(wp), W_ENDCOL(wp) - 1 - FDC_OFF,
		c2, c2, hl_attr(hl));
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_ENDCOL(wp) - 1 - FDC_OFF, W_ENDCOL(wp) - FDC_OFF,
		c1, c2, hl_attr(hl));
    }
    else
#endif
    {
#ifdef FEAT_CMDWIN
	if (cmdwin_type != 0 && wp == curwin)
	{
	    /* draw the cmdline character in the leftmost column */
	    n = 1;
	    if (n > wp->w_width)
		n = wp->w_width;
	    screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		    W_WINCOL(wp), (int)W_WINCOL(wp) + n,
		    cmdwin_type, ' ', hl_attr(HLF_AT));
	}
#endif
#ifdef FEAT_FOLDING
	if (fdc > 0)
	{
	    int	    nn = n + fdc;

	    /* draw the fold column at the left */
	    if (nn > W_WIDTH(wp))
		nn = W_WIDTH(wp);
	    screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		    W_WINCOL(wp) + n, (int)W_WINCOL(wp) + nn,
		    ' ', ' ', hl_attr(HLF_FC));
	    n = nn;
	}
#endif
#ifdef FEAT_SIGNS
	if (draw_signcolumn(wp))
	{
	    int	    nn = n + 2;

	    /* draw the sign column after the fold column */
	    if (nn > W_WIDTH(wp))
		nn = W_WIDTH(wp);
	    screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		    W_WINCOL(wp) + n, (int)W_WINCOL(wp) + nn,
		    ' ', ' ', hl_attr(HLF_SC));
	    n = nn;
	}
#endif
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + endrow,
		W_WINCOL(wp) + FDC_OFF, (int)W_ENDCOL(wp),
		c1, c2, hl_attr(hl));
    }
    set_empty_rows(wp, row);
}

#ifdef FEAT_SYN_HL
static int advance_color_col(int vcol, int **color_cols);

/*
 * Advance **color_cols and return TRUE when there are columns to draw.
 */
    static int
advance_color_col(int vcol, int **color_cols)
{
    while (**color_cols >= 0 && vcol > **color_cols)
	++*color_cols;
    return (**color_cols >= 0);
}
#endif

#ifdef FEAT_FOLDING
/*
 * Compute the width of the foldcolumn.  Based on 'foldcolumn' and how much
 * space is available for window "wp", minus "col".
 */
    static int
compute_foldcolumn(win_T *wp, int col)
{
    int fdc = wp->w_p_fdc;
    int wmw = wp == curwin && p_wmw == 0 ? 1 : p_wmw;
    int wwidth = W_WIDTH(wp);

    if (fdc > wwidth - (col + wmw))
	fdc = wwidth - (col + wmw);
    return fdc;
}

/*
 * Display one folded line.
 */
    static void
fold_line(
    win_T	*wp,
    long	fold_count,
    foldinfo_T	*foldinfo,
    linenr_T	lnum,
    int		row)
{
    char_u	buf[51];
    pos_T	*top, *bot;
    linenr_T	lnume = lnum + fold_count - 1;
    int		len;
    char_u	*text;
    int		fdc;
    int		col;
    int		txtcol;
    int		off = (int)(current_ScreenLine - ScreenLines);
    int		ri;

    /* Build the fold line:
     * 1. Add the cmdwin_type for the command-line window
     * 2. Add the 'foldcolumn'
     * 3. Add the 'number' or 'relativenumber' column
     * 4. Compose the text
     * 5. Add the text
     * 6. set highlighting for the Visual area an other text
     */
    col = 0;

    /*
     * 1. Add the cmdwin_type for the command-line window
     * Ignores 'rightleft', this window is never right-left.
     */
#ifdef FEAT_CMDWIN
    if (cmdwin_type != 0 && wp == curwin)
    {
	ScreenLines[off] = cmdwin_type;
	ScreenAttrs[off] = hl_attr(HLF_AT);
#ifdef FEAT_MBYTE
	if (enc_utf8)
	    ScreenLinesUC[off] = 0;
#endif
	++col;
    }
#endif

    /*
     * 2. Add the 'foldcolumn'
     *    Reduce the width when there is not enough space.
     */
    fdc = compute_foldcolumn(wp, col);
    if (fdc > 0)
    {
	fill_foldcolumn(buf, wp, TRUE, lnum);
#ifdef FEAT_RIGHTLEFT
	if (wp->w_p_rl)
	{
	    int		i;

	    copy_text_attr(off + W_WIDTH(wp) - fdc - col, buf, fdc,
							     hl_attr(HLF_FC));
	    /* reverse the fold column */
	    for (i = 0; i < fdc; ++i)
		ScreenLines[off + W_WIDTH(wp) - i - 1 - col] = buf[i];
	}
	else
#endif
	    copy_text_attr(off + col, buf, fdc, hl_attr(HLF_FC));
	col += fdc;
    }

#ifdef FEAT_RIGHTLEFT
# define RL_MEMSET(p, v, l)  if (wp->w_p_rl) \
				for (ri = 0; ri < l; ++ri) \
				   ScreenAttrs[off + (W_WIDTH(wp) - (p) - (l)) + ri] = v; \
			     else \
				for (ri = 0; ri < l; ++ri) \
				   ScreenAttrs[off + (p) + ri] = v
#else
# define RL_MEMSET(p, v, l)   for (ri = 0; ri < l; ++ri) \
				 ScreenAttrs[off + (p) + ri] = v
#endif

    /* Set all attributes of the 'number' or 'relativenumber' column and the
     * text */
    RL_MEMSET(col, hl_attr(HLF_FL), W_WIDTH(wp) - col);

#ifdef FEAT_SIGNS
    /* If signs are being displayed, add two spaces. */
    if (draw_signcolumn(wp))
    {
	len = W_WIDTH(wp) - col;
	if (len > 0)
	{
	    if (len > 2)
		len = 2;
# ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		/* the line number isn't reversed */
		copy_text_attr(off + W_WIDTH(wp) - len - col,
					(char_u *)"  ", len, hl_attr(HLF_FL));
	    else
# endif
		copy_text_attr(off + col, (char_u *)"  ", len, hl_attr(HLF_FL));
	    col += len;
	}
    }
#endif

    /*
     * 3. Add the 'number' or 'relativenumber' column
     */
    if (wp->w_p_nu || wp->w_p_rnu)
    {
	len = W_WIDTH(wp) - col;
	if (len > 0)
	{
	    int	    w = number_width(wp);
	    long    num;
	    char    *fmt = "%*ld ";

	    if (len > w + 1)
		len = w + 1;

	    if (wp->w_p_nu && !wp->w_p_rnu)
		/* 'number' + 'norelativenumber' */
		num = (long)lnum;
	    else
	    {
		/* 'relativenumber', don't use negative numbers */
		num = labs((long)get_cursor_rel_lnum(wp, lnum));
		if (num == 0 && wp->w_p_nu && wp->w_p_rnu)
		{
		    /* 'number' + 'relativenumber': cursor line shows absolute
		     * line number */
		    num = lnum;
		    fmt = "%-*ld ";
		}
	    }

	    sprintf((char *)buf, fmt, w, num);
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		/* the line number isn't reversed */
		copy_text_attr(off + W_WIDTH(wp) - len - col, buf, len,
							     hl_attr(HLF_FL));
	    else
#endif
		copy_text_attr(off + col, buf, len, hl_attr(HLF_FL));
	    col += len;
	}
    }

    /*
     * 4. Compose the folded-line string with 'foldtext', if set.
     */
    text = get_foldtext(wp, lnum, lnume, foldinfo, buf);

    txtcol = col;	/* remember where text starts */

    /*
     * 5. move the text to current_ScreenLine.  Fill up with "fill_fold".
     *    Right-left text is put in columns 0 - number-col, normal text is put
     *    in columns number-col - window-width.
     */
#ifdef FEAT_MBYTE
    if (has_mbyte)
    {
	int	cells;
	int	u8c, u8cc[MAX_MCO];
	int	i;
	int	idx;
	int	c_len;
	char_u	*p;
# ifdef FEAT_ARABIC
	int	prev_c = 0;		/* previous Arabic character */
	int	prev_c1 = 0;		/* first composing char for prev_c */
# endif

# ifdef FEAT_RIGHTLEFT
	if (wp->w_p_rl)
	    idx = off;
	else
# endif
	    idx = off + col;

	/* Store multibyte characters in ScreenLines[] et al. correctly. */
	for (p = text; *p != NUL; )
	{
	    cells = (*mb_ptr2cells)(p);
	    c_len = (*mb_ptr2len)(p);
	    if (col + cells > W_WIDTH(wp)
# ifdef FEAT_RIGHTLEFT
		    - (wp->w_p_rl ? col : 0)
# endif
		    )
		break;
	    ScreenLines[idx] = *p;
	    if (enc_utf8)
	    {
		u8c = utfc_ptr2char(p, u8cc);
		if (*p < 0x80 && u8cc[0] == 0)
		{
		    ScreenLinesUC[idx] = 0;
#ifdef FEAT_ARABIC
		    prev_c = u8c;
#endif
		}
		else
		{
#ifdef FEAT_ARABIC
		    if (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		    {
			/* Do Arabic shaping. */
			int	pc, pc1, nc;
			int	pcc[MAX_MCO];
			int	firstbyte = *p;

			/* The idea of what is the previous and next
			 * character depends on 'rightleft'. */
			if (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(p + c_len);
			    prev_c1 = u8cc[0];
			}
			else
			{
			    pc = utfc_ptr2char(p + c_len, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = u8c;

			u8c = arabic_shape(u8c, &firstbyte, &u8cc[0],
								 pc, pc1, nc);
			ScreenLines[idx] = firstbyte;
		    }
		    else
			prev_c = u8c;
#endif
		    /* Non-BMP character: display as ? or fullwidth ?. */
#ifdef UNICODE16
		    if (u8c >= 0x10000)
			ScreenLinesUC[idx] = (cells == 2) ? 0xff1f : (int)'?';
		    else
#endif
			ScreenLinesUC[idx] = u8c;
		    for (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][idx] = u8cc[i];
			if (u8cc[i] == 0)
			    break;
		    }
		}
		if (cells > 1)
		    ScreenLines[idx + 1] = 0;
	    }
	    else if (enc_dbcs == DBCS_JPNU && *p == 0x8e)
		/* double-byte single width character */
		ScreenLines2[idx] = p[1];
	    else if (cells > 1)
		/* double-width character */
		ScreenLines[idx + 1] = p[1];
	    col += cells;
	    idx += cells;
	    p += c_len;
	}
    }
    else
#endif
    {
	len = (int)STRLEN(text);
	if (len > W_WIDTH(wp) - col)
	    len = W_WIDTH(wp) - col;
	if (len > 0)
	{
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		STRNCPY(current_ScreenLine, text, len);
	    else
#endif
		STRNCPY(current_ScreenLine + col, text, len);
	    col += len;
	}
    }

    /* Fill the rest of the line with the fold filler */
#ifdef FEAT_RIGHTLEFT
    if (wp->w_p_rl)
	col -= txtcol;
#endif
    while (col < W_WIDTH(wp)
#ifdef FEAT_RIGHTLEFT
		    - (wp->w_p_rl ? txtcol : 0)
#endif
	    )
    {
#ifdef FEAT_MBYTE
	if (enc_utf8)
	{
	    if (fill_fold >= 0x80)
	    {
		ScreenLinesUC[off + col] = fill_fold;
		ScreenLinesC[0][off + col] = 0;
	    }
	    else
		ScreenLinesUC[off + col] = 0;
	}
#endif
	ScreenLines[off + col++] = fill_fold;
    }

    if (text != buf)
	vim_free(text);

    /*
     * 6. set highlighting for the Visual area an other text.
     * If all folded lines are in the Visual area, highlight the line.
     */
    if (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
	if (ltoreq(curwin->w_cursor, VIsual))
	{
	    /* Visual is after curwin->w_cursor */
	    top = &curwin->w_cursor;
	    bot = &VIsual;
	}
	else
	{
	    /* Visual is before curwin->w_cursor */
	    top = &VIsual;
	    bot = &curwin->w_cursor;
	}
	if (lnum >= top->lnum
		&& lnume <= bot->lnum
		&& (VIsual_mode != 'v'
		    || ((lnum > top->lnum
			    || (lnum == top->lnum
				&& top->col == 0))
			&& (lnume < bot->lnum
			    || (lnume == bot->lnum
				&& (bot->col - (*p_sel == 'e'))
		>= (colnr_T)STRLEN(ml_get_buf(wp->w_buffer, lnume, FALSE)))))))
	{
	    if (VIsual_mode == Ctrl_V)
	    {
		/* Visual block mode: highlight the chars part of the block */
		if (wp->w_old_cursor_fcol + txtcol < (colnr_T)W_WIDTH(wp))
		{
		    if (wp->w_old_cursor_lcol != MAXCOL
			     && wp->w_old_cursor_lcol + txtcol
						       < (colnr_T)W_WIDTH(wp))
			len = wp->w_old_cursor_lcol;
		    else
			len = W_WIDTH(wp) - txtcol;
		    RL_MEMSET(wp->w_old_cursor_fcol + txtcol, hl_attr(HLF_V),
					    len - (int)wp->w_old_cursor_fcol);
		}
	    }
	    else
	    {
		/* Set all attributes of the text */
		RL_MEMSET(txtcol, hl_attr(HLF_V), W_WIDTH(wp) - txtcol);
	    }
	}
    }

#ifdef FEAT_SYN_HL
    /* Show colorcolumn in the fold line, but let cursorcolumn override it. */
    if (wp->w_p_cc_cols)
    {
	int i = 0;
	int j = wp->w_p_cc_cols[i];
	int old_txtcol = txtcol;

	while (j > -1)
	{
	    txtcol += j;
	    if (wp->w_p_wrap)
		txtcol -= wp->w_skipcol;
	    else
		txtcol -= wp->w_leftcol;
	    if (txtcol >= 0 && txtcol < W_WIDTH(wp))
		ScreenAttrs[off + txtcol] = hl_combine_attr(
				    ScreenAttrs[off + txtcol], hl_attr(HLF_MC));
	    txtcol = old_txtcol;
	    j = wp->w_p_cc_cols[++i];
	}
    }

    /* Show 'cursorcolumn' in the fold line. */
    if (wp->w_p_cuc)
    {
	txtcol += wp->w_virtcol;
	if (wp->w_p_wrap)
	    txtcol -= wp->w_skipcol;
	else
	    txtcol -= wp->w_leftcol;
	if (txtcol >= 0 && txtcol < W_WIDTH(wp))
	    ScreenAttrs[off + txtcol] = hl_combine_attr(
				 ScreenAttrs[off + txtcol], hl_attr(HLF_CUC));
    }
#endif

    SCREEN_LINE(row + W_WINROW(wp), W_WINCOL(wp), (int)W_WIDTH(wp),
						     (int)W_WIDTH(wp), FALSE);

    /*
     * Update w_cline_height and w_cline_folded if the cursor line was
     * updated (saves a call to plines() later).
     */
    if (wp == curwin
	    && lnum <= curwin->w_cursor.lnum
	    && lnume >= curwin->w_cursor.lnum)
    {
	curwin->w_cline_row = row;
	curwin->w_cline_height = 1;
	curwin->w_cline_folded = TRUE;
	curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
    }
}

/*
 * Copy "buf[len]" to ScreenLines["off"] and set attributes to "attr".
 */
    static void
copy_text_attr(
    int		off,
    char_u	*buf,
    int		len,
    int		attr)
{
    int		i;

    mch_memmove(ScreenLines + off, buf, (size_t)len);
# ifdef FEAT_MBYTE
    if (enc_utf8)
	vim_memset(ScreenLinesUC + off, 0, sizeof(u8char_T) * (size_t)len);
# endif
    for (i = 0; i < len; ++i)
	ScreenAttrs[off + i] = attr;
}

/*
 * Fill the foldcolumn at "p" for window "wp".
 * Only to be called when 'foldcolumn' > 0.
 */
    static void
fill_foldcolumn(
    char_u	*p,
    win_T	*wp,
    int		closed,		/* TRUE of FALSE */
    linenr_T	lnum)		/* current line number */
{
    int		i = 0;
    int		level;
    int		first_level;
    int		empty;
    int		fdc = compute_foldcolumn(wp, 0);

    /* Init to all spaces. */
    vim_memset(p, ' ', (size_t)fdc);

    level = win_foldinfo.fi_level;
    if (level > 0)
    {
	/* If there is only one column put more info in it. */
	empty = (fdc == 1) ? 0 : 1;

	/* If the column is too narrow, we start at the lowest level that
	 * fits and use numbers to indicated the depth. */
	first_level = level - fdc - closed + 1 + empty;
	if (first_level < 1)
	    first_level = 1;

	for (i = 0; i + empty < fdc; ++i)
	{
	    if (win_foldinfo.fi_lnum == lnum
			      && first_level + i >= win_foldinfo.fi_low_level)
		p[i] = '-';
	    else if (first_level == 1)
		p[i] = '|';
	    else if (first_level + i <= 9)
		p[i] = '0' + first_level + i;
	    else
		p[i] = '>';
	    if (first_level + i == level)
		break;
	}
    }
    if (closed)
	p[i >= fdc ? i - 1 : i] = '+';
}
#endif /* FEAT_FOLDING */

/*
 * Display line "lnum" of window 'wp' on the screen.
 * Start at row "startrow", stop when "endrow" is reached.
 * wp->w_virtcol needs to be valid.
 *
 * Return the number of last row the line occupies.
 */
    static int
win_line(
    win_T	*wp,
    linenr_T	lnum,
    int		startrow,
    int		endrow,
    int		nochange UNUSED)	/* not updating for changed text */
{
    int		col;			/* visual column on screen */
    unsigned	off;			/* offset in ScreenLines/ScreenAttrs */
    int		c = 0;			/* init for GCC */
    long	vcol = 0;		/* virtual column (for tabs) */
#ifdef FEAT_LINEBREAK
    long	vcol_sbr = -1;		/* virtual column after showbreak */
#endif
    long	vcol_prev = -1;		/* "vcol" of previous character */
    char_u	*line;			/* current line */
    char_u	*ptr;			/* current position in "line" */
    int		row;			/* row in the window, excl w_winrow */
    int		screen_row;		/* row on the screen, incl w_winrow */

    char_u	extra[18];		/* "%ld" and 'fdc' must fit in here */
    int		n_extra = 0;		/* number of extra chars */
    char_u	*p_extra = NULL;	/* string of extra chars, plus NUL */
    char_u	*p_extra_free = NULL;   /* p_extra needs to be freed */
    int		c_extra = NUL;		/* extra chars, all the same */
    int		extra_attr = 0;		/* attributes when n_extra != 0 */
    static char_u *at_end_str = (char_u *)""; /* used for p_extra when
					   displaying lcs_eol at end-of-line */
    int		lcs_eol_one = lcs_eol;	/* lcs_eol until it's been used */
    int		lcs_prec_todo = lcs_prec;   /* lcs_prec until it's been used */

    /* saved "extra" items for when draw_state becomes WL_LINE (again) */
    int		saved_n_extra = 0;
    char_u	*saved_p_extra = NULL;
    int		saved_c_extra = 0;
    int		saved_char_attr = 0;

    int		n_attr = 0;		/* chars with special attr */
    int		saved_attr2 = 0;	/* char_attr saved for n_attr */
    int		n_attr3 = 0;		/* chars with overruling special attr */
    int		saved_attr3 = 0;	/* char_attr saved for n_attr3 */

    int		n_skip = 0;		/* nr of chars to skip for 'nowrap' */

    int		fromcol, tocol;		/* start/end of inverting */
    int		fromcol_prev = -2;	/* start of inverting after cursor */
    int		noinvcur = FALSE;	/* don't invert the cursor */
    pos_T	*top, *bot;
    int		lnum_in_visual_area = FALSE;
    pos_T	pos;
    long	v;

    int		char_attr = 0;		/* attributes for next character */
    int		attr_pri = FALSE;	/* char_attr has priority */
    int		area_highlighting = FALSE; /* Visual or incsearch highlighting
					      in this line */
    int		attr = 0;		/* attributes for area highlighting */
    int		area_attr = 0;		/* attributes desired by highlighting */
    int		search_attr = 0;	/* attributes desired by 'hlsearch' */
#ifdef FEAT_SYN_HL
    int		vcol_save_attr = 0;	/* saved attr for 'cursorcolumn' */
    int		syntax_attr = 0;	/* attributes desired by syntax */
    int		has_syntax = FALSE;	/* this buffer has syntax highl. */
    int		save_did_emsg;
    int		eol_hl_off = 0;		/* 1 if highlighted char after EOL */
    int		draw_color_col = FALSE;	/* highlight colorcolumn */
    int		*color_cols = NULL;	/* pointer to according columns array */
#endif
#ifdef FEAT_SPELL
    int		has_spell = FALSE;	/* this buffer has spell checking */
# define SPWORDLEN 150
    char_u	nextline[SPWORDLEN * 2];/* text with start of the next line */
    int		nextlinecol = 0;	/* column where nextline[] starts */
    int		nextline_idx = 0;	/* index in nextline[] where next line
					   starts */
    int		spell_attr = 0;		/* attributes desired by spelling */
    int		word_end = 0;		/* last byte with same spell_attr */
    static linenr_T  checked_lnum = 0;	/* line number for "checked_col" */
    static int	checked_col = 0;	/* column in "checked_lnum" up to which
					 * there are no spell errors */
    static int	cap_col = -1;		/* column to check for Cap word */
    static linenr_T capcol_lnum = 0;	/* line number where "cap_col" used */
    int		cur_checked_col = 0;	/* checked column for current line */
#endif
    int		extra_check;		/* has syntax or linebreak */
#ifdef FEAT_MBYTE
    int		multi_attr = 0;		/* attributes desired by multibyte */
    int		mb_l = 1;		/* multi-byte byte length */
    int		mb_c = 0;		/* decoded multi-byte character */
    int		mb_utf8 = FALSE;	/* screen char is UTF-8 char */
    int		u8cc[MAX_MCO];		/* composing UTF-8 chars */
#endif
#ifdef FEAT_DIFF
    int		filler_lines;		/* nr of filler lines to be drawn */
    int		filler_todo;		/* nr of filler lines still to do + 1 */
    hlf_T	diff_hlf = (hlf_T)0;	/* type of diff highlighting */
    int		change_start = MAXCOL;	/* first col of changed area */
    int		change_end = -1;	/* last col of changed area */
#endif
    colnr_T	trailcol = MAXCOL;	/* start of trailing spaces */
#ifdef FEAT_LINEBREAK
    int		need_showbreak = FALSE;
#endif
#if defined(FEAT_SIGNS) || (defined(FEAT_QUICKFIX) && defined(FEAT_WINDOWS)) \
	|| defined(FEAT_SYN_HL) || defined(FEAT_DIFF)
# define LINE_ATTR
    int		line_attr = 0;		/* attribute for the whole line */
#endif
#ifdef FEAT_SEARCH_EXTRA
    matchitem_T *cur;			/* points to the match list */
    match_T	*shl;			/* points to search_hl or a match */
    int		shl_flag;		/* flag to indicate whether search_hl
					   has been processed or not */
    int		pos_inprogress;		/* marks that position match search is
					   in progress */
    int		prevcol_hl_flag;	/* flag to indicate whether prevcol
					   equals startcol of search_hl or one
					   of the matches */
#endif
#ifdef FEAT_ARABIC
    int		prev_c = 0;		/* previous Arabic character */
    int		prev_c1 = 0;		/* first composing char for prev_c */
#endif
#if defined(LINE_ATTR)
    int		did_line_attr = 0;
#endif

    /* draw_state: items that are drawn in sequence: */
#define WL_START	0		/* nothing done yet */
#ifdef FEAT_CMDWIN
# define WL_CMDLINE	WL_START + 1	/* cmdline window column */
#else
# define WL_CMDLINE	WL_START
#endif
#ifdef FEAT_FOLDING
# define WL_FOLD	WL_CMDLINE + 1	/* 'foldcolumn' */
#else
# define WL_FOLD	WL_CMDLINE
#endif
#ifdef FEAT_SIGNS
# define WL_SIGN	WL_FOLD + 1	/* column for signs */
#else
# define WL_SIGN	WL_FOLD		/* column for signs */
#endif
#define WL_NR		WL_SIGN + 1	/* line number */
#ifdef FEAT_LINEBREAK
# define WL_BRI		WL_NR + 1	/* 'breakindent' */
#else
# define WL_BRI		WL_NR
#endif
#if defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
# define WL_SBR		WL_BRI + 1	/* 'showbreak' or 'diff' */
#else
# define WL_SBR		WL_BRI
#endif
#define WL_LINE		WL_SBR + 1	/* text in the line */
    int		draw_state = WL_START;	/* what to draw next */
#if defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
    int		feedback_col = 0;
    int		feedback_old_attr = -1;
#endif

#ifdef FEAT_CONCEAL
    int		syntax_flags	= 0;
    int		syntax_seqnr	= 0;
    int		prev_syntax_id	= 0;
    int		conceal_attr	= hl_attr(HLF_CONCEAL);
    int		is_concealing	= FALSE;
    int		boguscols	= 0;	/* nonexistent columns added to force
					   wrapping */
    int		vcol_off	= 0;	/* offset for concealed characters */
    int		did_wcol	= FALSE;
    int		match_conc	= FALSE; /* cchar for match functions */
    int		has_match_conc  = FALSE; /* match wants to conceal */
    int		old_boguscols   = 0;
# define VCOL_HLC (vcol - vcol_off)
# define FIX_FOR_BOGUSCOLS \
    { \
	n_extra += vcol_off; \
	vcol -= vcol_off; \
	vcol_off = 0; \
	col -= boguscols; \
	old_boguscols = boguscols; \
	boguscols = 0; \
    }
#else
# define VCOL_HLC (vcol)
#endif

    if (startrow > endrow)		/* past the end already! */
	return startrow;

    row = startrow;
    screen_row = row + W_WINROW(wp);

    /*
     * To speed up the loop below, set extra_check when there is linebreak,
     * trailing white space and/or syntax processing to be done.
     */
#ifdef FEAT_LINEBREAK
    extra_check = wp->w_p_lbr;
#else
    extra_check = 0;
#endif
#ifdef FEAT_SYN_HL
    if (syntax_present(wp) && !wp->w_s->b_syn_error)
    {
	/* Prepare for syntax highlighting in this line.  When there is an
	 * error, stop syntax highlighting. */
	save_did_emsg = did_emsg;
	did_emsg = FALSE;
	syntax_start(wp, lnum);
	if (did_emsg)
	    wp->w_s->b_syn_error = TRUE;
	else
	{
	    did_emsg = save_did_emsg;
	    has_syntax = TRUE;
	    extra_check = TRUE;
	}
    }

    /* Check for columns to display for 'colorcolumn'. */
    color_cols = wp->w_p_cc_cols;
    if (color_cols != NULL)
	draw_color_col = advance_color_col(VCOL_HLC, &color_cols);
#endif

#ifdef FEAT_SPELL
    if (wp->w_p_spell
	    && *wp->w_s->b_p_spl != NUL
	    && wp->w_s->b_langp.ga_len > 0
	    && *(char **)(wp->w_s->b_langp.ga_data) != NULL)
    {
	/* Prepare for spell checking. */
	has_spell = TRUE;
	extra_check = TRUE;

	/* Get the start of the next line, so that words that wrap to the next
	 * line are found too: "et<line-break>al.".
	 * Trick: skip a few chars for C/shell/Vim comments */
	nextline[SPWORDLEN] = NUL;
	if (lnum < wp->w_buffer->b_ml.ml_line_count)
	{
	    line = ml_get_buf(wp->w_buffer, lnum + 1, FALSE);
	    spell_cat_line(nextline + SPWORDLEN, line, SPWORDLEN);
	}

	/* When a word wrapped from the previous line the start of the current
	 * line is valid. */
	if (lnum == checked_lnum)
	    cur_checked_col = checked_col;
	checked_lnum = 0;

	/* When there was a sentence end in the previous line may require a
	 * word starting with capital in this line.  In line 1 always check
	 * the first word. */
	if (lnum != capcol_lnum)
	    cap_col = -1;
	if (lnum == 1)
	    cap_col = 0;
	capcol_lnum = 0;
    }
#endif

    /*
     * handle visual active in this window
     */
    fromcol = -10;
    tocol = MAXCOL;
    if (VIsual_active && wp->w_buffer == curwin->w_buffer)
    {
					/* Visual is after curwin->w_cursor */
	if (ltoreq(curwin->w_cursor, VIsual))
	{
	    top = &curwin->w_cursor;
	    bot = &VIsual;
	}
	else				/* Visual is before curwin->w_cursor */
	{
	    top = &VIsual;
	    bot = &curwin->w_cursor;
	}
	lnum_in_visual_area = (lnum >= top->lnum && lnum <= bot->lnum);
	if (VIsual_mode == Ctrl_V)	/* block mode */
	{
	    if (lnum_in_visual_area)
	    {
		fromcol = wp->w_old_cursor_fcol;
		tocol = wp->w_old_cursor_lcol;
	    }
	}
	else				/* non-block mode */
	{
	    if (lnum > top->lnum && lnum <= bot->lnum)
		fromcol = 0;
	    else if (lnum == top->lnum)
	    {
		if (VIsual_mode == 'V')	/* linewise */
		    fromcol = 0;
		else
		{
		    getvvcol(wp, top, (colnr_T *)&fromcol, NULL, NULL);
		    if (gchar_pos(top) == NUL)
			tocol = fromcol + 1;
		}
	    }
	    if (VIsual_mode != 'V' && lnum == bot->lnum)
	    {
		if (*p_sel == 'e' && bot->col == 0
#ifdef FEAT_VIRTUALEDIT
			&& bot->coladd == 0
#endif
		   )
		{
		    fromcol = -10;
		    tocol = MAXCOL;
		}
		else if (bot->col == MAXCOL)
		    tocol = MAXCOL;
		else
		{
		    pos = *bot;
		    if (*p_sel == 'e')
			getvvcol(wp, &pos, (colnr_T *)&tocol, NULL, NULL);
		    else
		    {
			getvvcol(wp, &pos, NULL, NULL, (colnr_T *)&tocol);
			++tocol;
		    }
		}
	    }
	}

	/* Check if the character under the cursor should not be inverted */
	if (!highlight_match && lnum == curwin->w_cursor.lnum && wp == curwin
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)
	    noinvcur = TRUE;

	/* if inverting in this line set area_highlighting */
	if (fromcol >= 0)
	{
	    area_highlighting = TRUE;
	    attr = hl_attr(HLF_V);
#if defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
	    if ((clip_star.available && !clip_star.owned
						     && clip_isautosel_star())
		    || (clip_plus.available && !clip_plus.owned
						    && clip_isautosel_plus()))
		attr = hl_attr(HLF_VNC);
#endif
	}
    }

    /*
     * handle 'incsearch' and ":s///c" highlighting
     */
    else if (highlight_match
	    && wp == curwin
	    && lnum >= curwin->w_cursor.lnum
	    && lnum <= curwin->w_cursor.lnum + search_match_lines)
    {
	if (lnum == curwin->w_cursor.lnum)
	    getvcol(curwin, &(curwin->w_cursor),
					     (colnr_T *)&fromcol, NULL, NULL);
	else
	    fromcol = 0;
	if (lnum == curwin->w_cursor.lnum + search_match_lines)
	{
	    pos.lnum = lnum;
	    pos.col = search_match_endcol;
	    getvcol(curwin, &pos, (colnr_T *)&tocol, NULL, NULL);
	}
	else
	    tocol = MAXCOL;
	/* do at least one character; happens when past end of line */
	if (fromcol == tocol)
	    tocol = fromcol + 1;
	area_highlighting = TRUE;
	attr = hl_attr(HLF_I);
    }

#ifdef FEAT_DIFF
    filler_lines = diff_check(wp, lnum);
    if (filler_lines < 0)
    {
	if (filler_lines == -1)
	{
	    if (diff_find_change(wp, lnum, &change_start, &change_end))
		diff_hlf = HLF_ADD;	/* added line */
	    else if (change_start == 0)
		diff_hlf = HLF_TXD;	/* changed text */
	    else
		diff_hlf = HLF_CHD;	/* changed line */
	}
	else
	    diff_hlf = HLF_ADD;		/* added line */
	filler_lines = 0;
	area_highlighting = TRUE;
    }
    if (lnum == wp->w_topline)
	filler_lines = wp->w_topfill;
    filler_todo = filler_lines;
#endif

#ifdef LINE_ATTR
# ifdef FEAT_SIGNS
    /* If this line has a sign with line highlighting set line_attr. */
    v = buf_getsigntype(wp->w_buffer, lnum, SIGN_LINEHL);
    if (v != 0)
	line_attr = sign_get_attr((int)v, TRUE);
# endif
# if defined(FEAT_QUICKFIX) && defined(FEAT_WINDOWS)
    /* Highlight the current line in the quickfix window. */
    if (bt_quickfix(wp->w_buffer) && qf_current_entry(wp) == lnum)
	line_attr = hl_attr(HLF_L);
# endif
    if (line_attr != 0)
	area_highlighting = TRUE;
#endif

    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
    ptr = line;

#ifdef FEAT_SPELL
    if (has_spell)
    {
	/* For checking first word with a capital skip white space. */
	if (cap_col == 0)
	    cap_col = (int)(skipwhite(line) - line);

	/* To be able to spell-check over line boundaries copy the end of the
	 * current line into nextline[].  Above the start of the next line was
	 * copied to nextline[SPWORDLEN]. */
	if (nextline[SPWORDLEN] == NUL)
	{
	    /* No next line or it is empty. */
	    nextlinecol = MAXCOL;
	    nextline_idx = 0;
	}
	else
	{
	    v = (long)STRLEN(line);
	    if (v < SPWORDLEN)
	    {
		/* Short line, use it completely and append the start of the
		 * next line. */
		nextlinecol = 0;
		mch_memmove(nextline, line, (size_t)v);
		STRMOVE(nextline + v, nextline + SPWORDLEN);
		nextline_idx = v + 1;
	    }
	    else
	    {
		/* Long line, use only the last SPWORDLEN bytes. */
		nextlinecol = v - SPWORDLEN;
		mch_memmove(nextline, line + nextlinecol, SPWORDLEN);
		nextline_idx = SPWORDLEN + 1;
	    }
	}
    }
#endif

    if (wp->w_p_list)
    {
	if (lcs_space || lcs_trail)
	    extra_check = TRUE;
	/* find start of trailing whitespace */
	if (lcs_trail)
	{
	    trailcol = (colnr_T)STRLEN(ptr);
	    while (trailcol > (colnr_T)0 && vim_iswhite(ptr[trailcol - 1]))
		--trailcol;
	    trailcol += (colnr_T) (ptr - line);
	}
    }

    /*
     * 'nowrap' or 'wrap' and a single line that doesn't fit: Advance to the
     * first character to be displayed.
     */
    if (wp->w_p_wrap)
	v = wp->w_skipcol;
    else
	v = wp->w_leftcol;
    if (v > 0)
    {
#ifdef FEAT_MBYTE
	char_u	*prev_ptr = ptr;
#endif
	while (vcol < v && *ptr != NUL)
	{
	    c = win_lbr_chartabsize(wp, line, ptr, (colnr_T)vcol, NULL);
	    vcol += c;
#ifdef FEAT_MBYTE
	    prev_ptr = ptr;
#endif
	    mb_ptr_adv(ptr);
	}

	/* When:
	 * - 'cuc' is set, or
	 * - 'colorcolumn' is set, or
	 * - 'virtualedit' is set, or
	 * - the visual mode is active,
	 * the end of the line may be before the start of the displayed part.
	 */
	if (vcol < v && (
#ifdef FEAT_SYN_HL
	     wp->w_p_cuc || draw_color_col ||
#endif
#ifdef FEAT_VIRTUALEDIT
	     virtual_active() ||
#endif
	     (VIsual_active && wp->w_buffer == curwin->w_buffer)))
	{
	    vcol = v;
	}

	/* Handle a character that's not completely on the screen: Put ptr at
	 * that character but skip the first few screen characters. */
	if (vcol > v)
	{
	    vcol -= c;
#ifdef FEAT_MBYTE
	    ptr = prev_ptr;
#else
	    --ptr;
#endif
	    n_skip = v - vcol;
	}

	/*
	 * Adjust for when the inverted text is before the screen,
	 * and when the start of the inverted text is before the screen.
	 */
	if (tocol <= vcol)
	    fromcol = 0;
	else if (fromcol >= 0 && fromcol < vcol)
	    fromcol = vcol;

#ifdef FEAT_LINEBREAK
	/* When w_skipcol is non-zero, first line needs 'showbreak' */
	if (wp->w_p_wrap)
	    need_showbreak = TRUE;
#endif
#ifdef FEAT_SPELL
	/* When spell checking a word we need to figure out the start of the
	 * word and if it's badly spelled or not. */
	if (has_spell)
	{
	    int		len;
	    colnr_T	linecol = (colnr_T)(ptr - line);
	    hlf_T	spell_hlf = HLF_COUNT;

	    pos = wp->w_cursor;
	    wp->w_cursor.lnum = lnum;
	    wp->w_cursor.col = linecol;
	    len = spell_move_to(wp, FORWARD, TRUE, TRUE, &spell_hlf);

	    /* spell_move_to() may call ml_get() and make "line" invalid */
	    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
	    ptr = line + linecol;

	    if (len == 0 || (int)wp->w_cursor.col > ptr - line)
	    {
		/* no bad word found at line start, don't check until end of a
		 * word */
		spell_hlf = HLF_COUNT;
		word_end = (int)(spell_to_word_end(ptr, wp) - line + 1);
	    }
	    else
	    {
		/* bad word found, use attributes until end of word */
		word_end = wp->w_cursor.col + len + 1;

		/* Turn index into actual attributes. */
		if (spell_hlf != HLF_COUNT)
		    spell_attr = highlight_attr[spell_hlf];
	    }
	    wp->w_cursor = pos;

# ifdef FEAT_SYN_HL
	    /* Need to restart syntax highlighting for this line. */
	    if (has_syntax)
		syntax_start(wp, lnum);
# endif
	}
#endif
    }

    /*
     * Correct highlighting for cursor that can't be disabled.
     * Avoids having to check this for each character.
     */
    if (fromcol >= 0)
    {
	if (noinvcur)
	{
	    if ((colnr_T)fromcol == wp->w_virtcol)
	    {
		/* highlighting starts at cursor, let it start just after the
		 * cursor */
		fromcol_prev = fromcol;
		fromcol = -1;
	    }
	    else if ((colnr_T)fromcol < wp->w_virtcol)
		/* restart highlighting after the cursor */
		fromcol_prev = wp->w_virtcol;
	}
	if (fromcol >= tocol)
	    fromcol = -1;
    }

#ifdef FEAT_SEARCH_EXTRA
    /*
     * Handle highlighting the last used search pattern and matches.
     * Do this for both search_hl and the match list.
     */
    cur = wp->w_match_head;
    shl_flag = FALSE;
    while (cur != NULL || shl_flag == FALSE)
    {
	if (shl_flag == FALSE)
	{
	    shl = &search_hl;
	    shl_flag = TRUE;
	}
	else
	    shl = &cur->hl;
	shl->startcol = MAXCOL;
	shl->endcol = MAXCOL;
	shl->attr_cur = 0;
	v = (long)(ptr - line);
	if (cur != NULL)
	    cur->pos.cur = 0;
	next_search_hl(wp, shl, lnum, (colnr_T)v, cur);

	/* Need to get the line again, a multi-line regexp may have made it
	 * invalid. */
	line = ml_get_buf(wp->w_buffer, lnum, FALSE);
	ptr = line + v;

	if (shl->lnum != 0 && shl->lnum <= lnum)
	{
	    if (shl->lnum == lnum)
		shl->startcol = shl->rm.startpos[0].col;
	    else
		shl->startcol = 0;
	    if (lnum == shl->lnum + shl->rm.endpos[0].lnum
						- shl->rm.startpos[0].lnum)
		shl->endcol = shl->rm.endpos[0].col;
	    else
		shl->endcol = MAXCOL;
	    /* Highlight one character for an empty match. */
	    if (shl->startcol == shl->endcol)
	    {
#ifdef FEAT_MBYTE
		if (has_mbyte && line[shl->endcol] != NUL)
		    shl->endcol += (*mb_ptr2len)(line + shl->endcol);
		else
#endif
		    ++shl->endcol;
	    }
	    if ((long)shl->startcol < v)  /* match at leftcol */
	    {
		shl->attr_cur = shl->attr;
		search_attr = shl->attr;
	    }
	    area_highlighting = TRUE;
	}
	if (shl != &search_hl && cur != NULL)
	    cur = cur->next;
    }
#endif

#ifdef FEAT_SYN_HL
    /* Cursor line highlighting for 'cursorline' in the current window.  Not
     * when Visual mode is active, because it's not clear what is selected
     * then. */
    if (wp->w_p_cul && lnum == wp->w_cursor.lnum
					 && !(wp == curwin && VIsual_active))
    {
	line_attr = hl_attr(HLF_CUL);
	area_highlighting = TRUE;
    }
#endif

    off = (unsigned)(current_ScreenLine - ScreenLines);
    col = 0;
#ifdef FEAT_RIGHTLEFT
    if (wp->w_p_rl)
    {
	/* Rightleft window: process the text in the normal direction, but put
	 * it in current_ScreenLine[] from right to left.  Start at the
	 * rightmost column of the window. */
	col = W_WIDTH(wp) - 1;
	off += col;
    }
#endif

    /*
     * Repeat for the whole displayed line.
     */
    for (;;)
    {
#ifdef FEAT_CONCEAL
	has_match_conc = FALSE;
#endif
	/* Skip this quickly when working on the text. */
	if (draw_state != WL_LINE)
	{
#ifdef FEAT_CMDWIN
	    if (draw_state == WL_CMDLINE - 1 && n_extra == 0)
	    {
		draw_state = WL_CMDLINE;
		if (cmdwin_type != 0 && wp == curwin)
		{
		    /* Draw the cmdline character. */
		    n_extra = 1;
		    c_extra = cmdwin_type;
		    char_attr = hl_attr(HLF_AT);
		}
	    }
#endif

#ifdef FEAT_FOLDING
	    if (draw_state == WL_FOLD - 1 && n_extra == 0)
	    {
		int fdc = compute_foldcolumn(wp, 0);

		draw_state = WL_FOLD;
		if (fdc > 0)
		{
		    /* Draw the 'foldcolumn'. */
		    fill_foldcolumn(extra, wp, FALSE, lnum);
		    n_extra = fdc;
		    p_extra = extra;
		    p_extra[n_extra] = NUL;
		    c_extra = NUL;
		    char_attr = hl_attr(HLF_FC);
		}
	    }
#endif

#ifdef FEAT_SIGNS
	    if (draw_state == WL_SIGN - 1 && n_extra == 0)
	    {
		draw_state = WL_SIGN;
		/* Show the sign column when there are any signs in this
		 * buffer or when using Netbeans. */
		if (draw_signcolumn(wp))
		{
		    int	text_sign;
# ifdef FEAT_SIGN_ICONS
		    int	icon_sign;
# endif

		    /* Draw two cells with the sign value or blank. */
		    c_extra = ' ';
		    char_attr = hl_attr(HLF_SC);
		    n_extra = 2;

		    if (row == startrow
#ifdef FEAT_DIFF
			    + filler_lines && filler_todo <= 0
#endif
			    )
		    {
			text_sign = buf_getsigntype(wp->w_buffer, lnum,
								   SIGN_TEXT);
# ifdef FEAT_SIGN_ICONS
			icon_sign = buf_getsigntype(wp->w_buffer, lnum,
								   SIGN_ICON);
			if (gui.in_use && icon_sign != 0)
			{
			    /* Use the image in this position. */
			    c_extra = SIGN_BYTE;
#  ifdef FEAT_NETBEANS_INTG
			    if (buf_signcount(wp->w_buffer, lnum) > 1)
				c_extra = MULTISIGN_BYTE;
#  endif
			    char_attr = icon_sign;
			}
			else
# endif
			    if (text_sign != 0)
			{
			    p_extra = sign_get_text(text_sign);
			    if (p_extra != NULL)
			    {
				c_extra = NUL;
				n_extra = (int)STRLEN(p_extra);
			    }
			    char_attr = sign_get_attr(text_sign, FALSE);
			}
		    }
		}
	    }
#endif

	    if (draw_state == WL_NR - 1 && n_extra == 0)
	    {
		draw_state = WL_NR;
		/* Display the absolute or relative line number. After the
		 * first fill with blanks when the 'n' flag isn't in 'cpo' */
		if ((wp->w_p_nu || wp->w_p_rnu)
			&& (row == startrow
#ifdef FEAT_DIFF
			    + filler_lines
#endif
			    || vim_strchr(p_cpo, CPO_NUMCOL) == NULL))
		{
		    /* Draw the line number (empty space after wrapping). */
		    if (row == startrow
#ifdef FEAT_DIFF
			    + filler_lines
#endif
			    )
		    {
			long num;
			char *fmt = "%*ld ";

			if (wp->w_p_nu && !wp->w_p_rnu)
			    /* 'number' + 'norelativenumber' */
			    num = (long)lnum;
			else
			{
			    /* 'relativenumber', don't use negative numbers */
			    num = labs((long)get_cursor_rel_lnum(wp, lnum));
			    if (num == 0 && wp->w_p_nu && wp->w_p_rnu)
			    {
				/* 'number' + 'relativenumber' */
				num = lnum;
				fmt = "%-*ld ";
			    }
			}

			sprintf((char *)extra, fmt,
						number_width(wp), num);
			if (wp->w_skipcol > 0)
			    for (p_extra = extra; *p_extra == ' '; ++p_extra)
				*p_extra = '-';
#ifdef FEAT_RIGHTLEFT
			if (wp->w_p_rl)		    /* reverse line numbers */
			    rl_mirror(extra);
#endif
			p_extra = extra;
			c_extra = NUL;
		    }
		    else
			c_extra = ' ';
		    n_extra = number_width(wp) + 1;
		    char_attr = hl_attr(HLF_N);
#ifdef FEAT_SYN_HL
		    /* When 'cursorline' is set highlight the line number of
		     * the current line differently.
		     * TODO: Can we use CursorLine instead of CursorLineNr
		     * when CursorLineNr isn't set? */
		    if ((wp->w_p_cul || wp->w_p_rnu)
						 && lnum == wp->w_cursor.lnum)
			char_attr = hl_attr(HLF_CLN);
#endif
		}
	    }

#ifdef FEAT_LINEBREAK
	    if (wp->w_p_brisbr && draw_state == WL_BRI - 1
					     && n_extra == 0 && *p_sbr != NUL)
		/* draw indent after showbreak value */
		draw_state = WL_BRI;
	    else if (wp->w_p_brisbr && draw_state == WL_SBR && n_extra == 0)
		/* After the showbreak, draw the breakindent */
		draw_state = WL_BRI - 1;

	    /* draw 'breakindent': indent wrapped text accordingly */
	    if (draw_state == WL_BRI - 1 && n_extra == 0)
	    {
		draw_state = WL_BRI;
		if (wp->w_p_bri && n_extra == 0 && row != startrow
# ifdef FEAT_DIFF
			&& filler_lines == 0
# endif
		   )
		{
		    char_attr = 0; /* was: hl_attr(HLF_AT); */
# ifdef FEAT_DIFF
		    if (diff_hlf != (hlf_T)0)
		    {
			char_attr = hl_attr(diff_hlf);
#  ifdef FEAT_SYN_HL
			if (wp->w_p_cul && lnum == wp->w_cursor.lnum)
			    char_attr = hl_combine_attr(char_attr,
							    hl_attr(HLF_CUL));
#  endif
		    }
# endif
		    p_extra = NULL;
		    c_extra = ' ';
		    n_extra = get_breakindent_win(wp,
				       ml_get_buf(wp->w_buffer, lnum, FALSE));
		    /* Correct end of highlighted area for 'breakindent',
		     * required when 'linebreak' is also set. */
		    if (tocol == vcol)
			tocol += n_extra;
		}
	    }
#endif

#if defined(FEAT_LINEBREAK) || defined(FEAT_DIFF)
	    if (draw_state == WL_SBR - 1 && n_extra == 0)
	    {
		draw_state = WL_SBR;
# ifdef FEAT_DIFF
		if (filler_todo > 0)
		{
		    /* Draw "deleted" diff line(s). */
		    if (char2cells(fill_diff) > 1)
			c_extra = '-';
		    else
			c_extra = fill_diff;
#  ifdef FEAT_RIGHTLEFT
		    if (wp->w_p_rl)
			n_extra = col + 1;
		    else
#  endif
			n_extra = W_WIDTH(wp) - col;
		    char_attr = hl_attr(HLF_DED);
		}
# endif
# ifdef FEAT_LINEBREAK
		if (*p_sbr != NUL && need_showbreak)
		{
		    /* Draw 'showbreak' at the start of each broken line. */
		    p_extra = p_sbr;
		    c_extra = NUL;
		    n_extra = (int)STRLEN(p_sbr);
		    char_attr = hl_attr(HLF_AT);
		    need_showbreak = FALSE;
		    vcol_sbr = vcol + MB_CHARLEN(p_sbr);
		    /* Correct end of highlighted area for 'showbreak',
		     * required when 'linebreak' is also set. */
		    if (tocol == vcol)
			tocol += n_extra;
#ifdef FEAT_SYN_HL
		    /* combine 'showbreak' with 'cursorline' */
		    if (wp->w_p_cul && lnum == wp->w_cursor.lnum)
			char_attr = hl_combine_attr(char_attr,
							    hl_attr(HLF_CUL));
#endif
		}
# endif
	    }
#endif

	    if (draw_state == WL_LINE - 1 && n_extra == 0)
	    {
		draw_state = WL_LINE;
		if (saved_n_extra)
		{
		    /* Continue item from end of wrapped line. */
		    n_extra = saved_n_extra;
		    c_extra = saved_c_extra;
		    p_extra = saved_p_extra;
		    char_attr = saved_char_attr;
		}
		else
		    char_attr = 0;
	    }
	}

	/* When still displaying '$' of change command, stop at cursor */
	if (dollar_vcol >= 0 && wp == curwin
		   && lnum == wp->w_cursor.lnum && vcol >= (long)wp->w_virtcol
#ifdef FEAT_DIFF
				   && filler_todo <= 0
#endif
		)
	{
	    SCREEN_LINE(screen_row, W_WINCOL(wp), col, -(int)W_WIDTH(wp),
								  wp->w_p_rl);
	    /* Pretend we have finished updating the window.  Except when
	     * 'cursorcolumn' is set. */
#ifdef FEAT_SYN_HL
	    if (wp->w_p_cuc)
		row = wp->w_cline_row + wp->w_cline_height;
	    else
#endif
		row = wp->w_height;
	    break;
	}

	if (draw_state == WL_LINE && area_highlighting)
	{
	    /* handle Visual or match highlighting in this line */
	    if (vcol == fromcol
#ifdef FEAT_MBYTE
		    || (has_mbyte && vcol + 1 == fromcol && n_extra == 0
			&& (*mb_ptr2cells)(ptr) > 1)
#endif
		    || ((int)vcol_prev == fromcol_prev
			&& vcol_prev < vcol	/* not at margin */
			&& vcol < tocol))
		area_attr = attr;		/* start highlighting */
	    else if (area_attr != 0
		    && (vcol == tocol
			|| (noinvcur && (colnr_T)vcol == wp->w_virtcol)))
		area_attr = 0;			/* stop highlighting */

#ifdef FEAT_SEARCH_EXTRA
	    if (!n_extra)
	    {
		/*
		 * Check for start/end of search pattern match.
		 * After end, check for start/end of next match.
		 * When another match, have to check for start again.
		 * Watch out for matching an empty string!
		 * Do this for 'search_hl' and the match list (ordered by
		 * priority).
		 */
		v = (long)(ptr - line);
		cur = wp->w_match_head;
		shl_flag = FALSE;
		while (cur != NULL || shl_flag == FALSE)
		{
		    if (shl_flag == FALSE
			    && ((cur != NULL
				    && cur->priority > SEARCH_HL_PRIORITY)
				|| cur == NULL))
		    {
			shl = &search_hl;
			shl_flag = TRUE;
		    }
		    else
			shl = &cur->hl;
		    if (cur != NULL)
			cur->pos.cur = 0;
		    pos_inprogress = TRUE;
		    while (shl->rm.regprog != NULL
					   || (cur != NULL && pos_inprogress))
		    {
			if (shl->startcol != MAXCOL
				&& v >= (long)shl->startcol
				&& v < (long)shl->endcol)
			{
#ifdef FEAT_MBYTE
			    int tmp_col = v + MB_PTR2LEN(ptr);

			    if (shl->endcol < tmp_col)
				shl->endcol = tmp_col;
#endif
			    shl->attr_cur = shl->attr;
#ifdef FEAT_CONCEAL
			    if (cur != NULL && syn_name2id((char_u *)"Conceal")
							       == cur->hlg_id)
			    {
				has_match_conc = TRUE;
				match_conc = cur->conceal_char;
			    }
			    else
				has_match_conc = match_conc = FALSE;
#endif
			}
			else if (v == (long)shl->endcol)
			{
			    shl->attr_cur = 0;
#ifdef FEAT_CONCEAL
			    prev_syntax_id = 0;
#endif
			    next_search_hl(wp, shl, lnum, (colnr_T)v, cur);
			    pos_inprogress = cur == NULL || cur->pos.cur == 0
							       ? FALSE : TRUE;

			    /* Need to get the line again, a multi-line regexp
			     * may have made it invalid. */
			    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
			    ptr = line + v;

			    if (shl->lnum == lnum)
			    {
				shl->startcol = shl->rm.startpos[0].col;
				if (shl->rm.endpos[0].lnum == 0)
				    shl->endcol = shl->rm.endpos[0].col;
				else
				    shl->endcol = MAXCOL;

				if (shl->startcol == shl->endcol)
				{
				    /* highlight empty match, try again after
				     * it */
#ifdef FEAT_MBYTE
				    if (has_mbyte)
					shl->endcol += (*mb_ptr2len)(line
							       + shl->endcol);
				    else
#endif
					++shl->endcol;
				}

				/* Loop to check if the match starts at the
				 * current position */
				continue;
			    }
			}
			break;
		    }
		    if (shl != &search_hl && cur != NULL)
			cur = cur->next;
		}

		/* Use attributes from match with highest priority among
		 * 'search_hl' and the match list. */
		search_attr = search_hl.attr_cur;
		cur = wp->w_match_head;
		shl_flag = FALSE;
		while (cur != NULL || shl_flag == FALSE)
		{
		    if (shl_flag == FALSE
			    && ((cur != NULL
				    && cur->priority > SEARCH_HL_PRIORITY)
				|| cur == NULL))
		    {
			shl = &search_hl;
			shl_flag = TRUE;
		    }
		    else
			shl = &cur->hl;
		    if (shl->attr_cur != 0)
			search_attr = shl->attr_cur;
		    if (shl != &search_hl && cur != NULL)
			cur = cur->next;
		}
	    }
#endif

#ifdef FEAT_DIFF
	    if (diff_hlf != (hlf_T)0)
	    {
		if (diff_hlf == HLF_CHD && ptr - line >= change_start
							      && n_extra == 0)
		    diff_hlf = HLF_TXD;		/* changed text */
		if (diff_hlf == HLF_TXD && ptr - line > change_end
							      && n_extra == 0)
		    diff_hlf = HLF_CHD;		/* changed line */
		line_attr = hl_attr(diff_hlf);
		if (wp->w_p_cul && lnum == wp->w_cursor.lnum)
		    line_attr = hl_combine_attr(line_attr, hl_attr(HLF_CUL));
	    }
#endif

	    /* Decide which of the highlight attributes to use. */
	    attr_pri = TRUE;
#ifdef LINE_ATTR
	    if (area_attr != 0)
		char_attr = hl_combine_attr(line_attr, area_attr);
	    else if (search_attr != 0)
		char_attr = hl_combine_attr(line_attr, search_attr);
		/* Use line_attr when not in the Visual or 'incsearch' area
		 * (area_attr may be 0 when "noinvcur" is set). */
	    else if (line_attr != 0 && ((fromcol == -10 && tocol == MAXCOL)
				|| vcol < fromcol || vcol_prev < fromcol_prev
				|| vcol >= tocol))
		char_attr = line_attr;
#else
	    if (area_attr != 0)
		char_attr = area_attr;
	    else if (search_attr != 0)
		char_attr = search_attr;
#endif
	    else
	    {
		attr_pri = FALSE;
#ifdef FEAT_SYN_HL
		if (has_syntax)
		    char_attr = syntax_attr;
		else
#endif
		    char_attr = 0;
	    }
	}

	/*
	 * Get the next character to put on the screen.
	 */
	/*
	 * The "p_extra" points to the extra stuff that is inserted to
	 * represent special characters (non-printable stuff) and other
	 * things.  When all characters are the same, c_extra is used.
	 * "p_extra" must end in a NUL to avoid mb_ptr2len() reads past
	 * "p_extra[n_extra]".
	 * For the '$' of the 'list' option, n_extra == 1, p_extra == "".
	 */
	if (n_extra > 0)
	{
	    if (c_extra != NUL)
	    {
		c = c_extra;
#ifdef FEAT_MBYTE
		mb_c = c;	/* doesn't handle non-utf-8 multi-byte! */
		if (enc_utf8 && (*mb_char2len)(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		else
		    mb_utf8 = FALSE;
#endif
	    }
	    else
	    {
		c = *p_extra;
#ifdef FEAT_MBYTE
		if (has_mbyte)
		{
		    mb_c = c;
		    if (enc_utf8)
		    {
			/* If the UTF-8 character is more than one byte:
			 * Decode it into "mb_c". */
			mb_l = (*mb_ptr2len)(p_extra);
			mb_utf8 = FALSE;
			if (mb_l > n_extra)
			    mb_l = 1;
			else if (mb_l > 1)
			{
			    mb_c = utfc_ptr2char(p_extra, u8cc);
			    mb_utf8 = TRUE;
			    c = 0xc0;
			}
		    }
		    else
		    {
			/* if this is a DBCS character, put it in "mb_c" */
			mb_l = MB_BYTE2LEN(c);
			if (mb_l >= n_extra)
			    mb_l = 1;
			else if (mb_l > 1)
			    mb_c = (c << 8) + p_extra[1];
		    }
		    if (mb_l == 0)  /* at the NUL at end-of-line */
			mb_l = 1;

		    /* If a double-width char doesn't fit display a '>' in the
		     * last column. */
		    if ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (col <= 0) :
# endif
				    (col >= W_WIDTH(wp) - 1))
			    && (*mb_char2cells)(mb_c) == 2)
		    {
			c = '>';
			mb_c = c;
			mb_l = 1;
			mb_utf8 = FALSE;
			multi_attr = hl_attr(HLF_AT);
			/* put the pointer back to output the double-width
			 * character at the start of the next line. */
			++n_extra;
			--p_extra;
		    }
		    else
		    {
			n_extra -= mb_l - 1;
			p_extra += mb_l - 1;
		    }
		}
#endif
		++p_extra;
	    }
	    --n_extra;
	}
	else
	{
	    if (p_extra_free != NULL)
	    {
		vim_free(p_extra_free);
		p_extra_free = NULL;
	    }
	    /*
	     * Get a character from the line itself.
	     */
	    c = *ptr;
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		mb_c = c;
		if (enc_utf8)
		{
		    /* If the UTF-8 character is more than one byte: Decode it
		     * into "mb_c". */
		    mb_l = (*mb_ptr2len)(ptr);
		    mb_utf8 = FALSE;
		    if (mb_l > 1)
		    {
			mb_c = utfc_ptr2char(ptr, u8cc);
			/* Overlong encoded ASCII or ASCII with composing char
			 * is displayed normally, except a NUL. */
			if (mb_c < 0x80)
			    c = mb_c;
			mb_utf8 = TRUE;

			/* At start of the line we can have a composing char.
			 * Draw it as a space with a composing char. */
			if (utf_iscomposing(mb_c))
			{
			    int i;

			    for (i = Screen_mco - 1; i > 0; --i)
				u8cc[i] = u8cc[i - 1];
			    u8cc[0] = mb_c;
			    mb_c = ' ';
			}
		    }

		    if ((mb_l == 1 && c >= 0x80)
			    || (mb_l >= 1 && mb_c == 0)
			    || (mb_l > 1 && (!vim_isprintc(mb_c)
# ifdef UNICODE16
							 || mb_c >= 0x10000
# endif
							 )))
		    {
			/*
			 * Illegal UTF-8 byte: display as <xx>.
			 * Non-BMP character : display as ? or fullwidth ?.
			 */
# ifdef UNICODE16
			if (mb_c < 0x10000)
# endif
			{
			    transchar_hex(extra, mb_c);
# ifdef FEAT_RIGHTLEFT
			    if (wp->w_p_rl)		/* reverse */
				rl_mirror(extra);
# endif
			}
# ifdef UNICODE16
			else if (utf_char2cells(mb_c) != 2)
			    STRCPY(extra, "?");
			else
			    /* 0xff1f in UTF-8: full-width '?' */
			    STRCPY(extra, "\357\274\237");
# endif

			p_extra = extra;
			c = *p_extra;
			mb_c = mb_ptr2char_adv(&p_extra);
			mb_utf8 = (c >= 0x80);
			n_extra = (int)STRLEN(p_extra);
			c_extra = NUL;
			if (area_attr == 0 && search_attr == 0)
			{
			    n_attr = n_extra + 1;
			    extra_attr = hl_attr(HLF_8);
			    saved_attr2 = char_attr; /* save current attr */
			}
		    }
		    else if (mb_l == 0)  /* at the NUL at end-of-line */
			mb_l = 1;
#ifdef FEAT_ARABIC
		    else if (p_arshape && !p_tbidi && ARABIC_CHAR(mb_c))
		    {
			/* Do Arabic shaping. */
			int	pc, pc1, nc;
			int	pcc[MAX_MCO];

			/* The idea of what is the previous and next
			 * character depends on 'rightleft'. */
			if (wp->w_p_rl)
			{
			    pc = prev_c;
			    pc1 = prev_c1;
			    nc = utf_ptr2char(ptr + mb_l);
			    prev_c1 = u8cc[0];
			}
			else
			{
			    pc = utfc_ptr2char(ptr + mb_l, pcc);
			    nc = prev_c;
			    pc1 = pcc[0];
			}
			prev_c = mb_c;

			mb_c = arabic_shape(mb_c, &c, &u8cc[0], pc, pc1, nc);
		    }
		    else
			prev_c = mb_c;
#endif
		}
		else	/* enc_dbcs */
		{
		    mb_l = MB_BYTE2LEN(c);
		    if (mb_l == 0)  /* at the NUL at end-of-line */
			mb_l = 1;
		    else if (mb_l > 1)
		    {
			/* We assume a second byte below 32 is illegal.
			 * Hopefully this is OK for all double-byte encodings!
			 */
			if (ptr[1] >= 32)
			    mb_c = (c << 8) + ptr[1];
			else
			{
			    if (ptr[1] == NUL)
			    {
				/* head byte at end of line */
				mb_l = 1;
				transchar_nonprint(extra, c);
			    }
			    else
			    {
				/* illegal tail byte */
				mb_l = 2;
				STRCPY(extra, "XX");
			    }
			    p_extra = extra;
			    n_extra = (int)STRLEN(extra) - 1;
			    c_extra = NUL;
			    c = *p_extra++;
			    if (area_attr == 0 && search_attr == 0)
			    {
				n_attr = n_extra + 1;
				extra_attr = hl_attr(HLF_8);
				saved_attr2 = char_attr; /* save current attr */
			    }
			    mb_c = c;
			}
		    }
		}
		/* If a double-width char doesn't fit display a '>' in the
		 * last column; the character is displayed at the start of the
		 * next line. */
		if ((
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (col <= 0) :
# endif
				(col >= W_WIDTH(wp) - 1))
			&& (*mb_char2cells)(mb_c) == 2)
		{
		    c = '>';
		    mb_c = c;
		    mb_utf8 = FALSE;
		    mb_l = 1;
		    multi_attr = hl_attr(HLF_AT);
		    /* Put pointer back so that the character will be
		     * displayed at the start of the next line. */
		    --ptr;
		}
		else if (*ptr != NUL)
		    ptr += mb_l - 1;

		/* If a double-width char doesn't fit at the left side display
		 * a '<' in the first column.  Don't do this for unprintable
		 * characters. */
		if (n_skip > 0 && mb_l > 1 && n_extra == 0)
		{
		    n_extra = 1;
		    c_extra = MB_FILLER_CHAR;
		    c = ' ';
		    if (area_attr == 0 && search_attr == 0)
		    {
			n_attr = n_extra + 1;
			extra_attr = hl_attr(HLF_AT);
			saved_attr2 = char_attr; /* save current attr */
		    }
		    mb_c = c;
		    mb_utf8 = FALSE;
		    mb_l = 1;
		}

	    }
#endif
	    ++ptr;

	    if (extra_check)
	    {
#ifdef FEAT_SPELL
		int	can_spell = TRUE;
#endif

#ifdef FEAT_SYN_HL
		/* Get syntax attribute, unless still at the start of the line
		 * (double-wide char that doesn't fit). */
		v = (long)(ptr - line);
		if (has_syntax && v > 0)
		{
		    /* Get the syntax attribute for the character.  If there
		     * is an error, disable syntax highlighting. */
		    save_did_emsg = did_emsg;
		    did_emsg = FALSE;

		    syntax_attr = get_syntax_attr((colnr_T)v - 1,
# ifdef FEAT_SPELL
						has_spell ? &can_spell :
# endif
						NULL, FALSE);

		    if (did_emsg)
		    {
			wp->w_s->b_syn_error = TRUE;
			has_syntax = FALSE;
		    }
		    else
			did_emsg = save_did_emsg;

		    /* Need to get the line again, a multi-line regexp may
		     * have made it invalid. */
		    line = ml_get_buf(wp->w_buffer, lnum, FALSE);
		    ptr = line + v;

		    if (!attr_pri)
			char_attr = syntax_attr;
		    else
			char_attr = hl_combine_attr(syntax_attr, char_attr);
# ifdef FEAT_CONCEAL
		    /* no concealing past the end of the line, it interferes
		     * with line highlighting */
		    if (c == NUL)
			syntax_flags = 0;
		    else
			syntax_flags = get_syntax_info(&syntax_seqnr);
# endif
		}
#endif

#ifdef FEAT_SPELL
		/* Check spelling (unless at the end of the line).
		 * Only do this when there is no syntax highlighting, the
		 * @Spell cluster is not used or the current syntax item
		 * contains the @Spell cluster. */
		if (has_spell && v >= word_end && v > cur_checked_col)
		{
		    spell_attr = 0;
# ifdef FEAT_SYN_HL
		    if (!attr_pri)
			char_attr = syntax_attr;
# endif
		    if (c != 0 && (
# ifdef FEAT_SYN_HL
				!has_syntax ||
# endif
				can_spell))
		    {
			char_u	*prev_ptr, *p;
			int	len;
			hlf_T	spell_hlf = HLF_COUNT;
# ifdef FEAT_MBYTE
			if (has_mbyte)
			{
			    prev_ptr = ptr - mb_l;
			    v -= mb_l - 1;
			}
			else
# endif
			    prev_ptr = ptr - 1;

			/* Use nextline[] if possible, it has the start of the
			 * next line concatenated. */
			if ((prev_ptr - line) - nextlinecol >= 0)
			    p = nextline + (prev_ptr - line) - nextlinecol;
			else
			    p = prev_ptr;
			cap_col -= (int)(prev_ptr - line);
			len = spell_check(wp, p, &spell_hlf, &cap_col,
								    nochange);
			word_end = v + len;

			/* In Insert mode only highlight a word that
			 * doesn't touch the cursor. */
			if (spell_hlf != HLF_COUNT
				&& (State & INSERT) != 0
				&& wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col >=
						    (colnr_T)(prev_ptr - line)
				&& wp->w_cursor.col < (colnr_T)word_end)
			{
			    spell_hlf = HLF_COUNT;
			    spell_redraw_lnum = lnum;
			}

			if (spell_hlf == HLF_COUNT && p != prev_ptr
				       && (p - nextline) + len > nextline_idx)
			{
			    /* Remember that the good word continues at the
			     * start of the next line. */
			    checked_lnum = lnum + 1;
			    checked_col = (int)((p - nextline) + len - nextline_idx);
			}

			/* Turn index into actual attributes. */
			if (spell_hlf != HLF_COUNT)
			    spell_attr = highlight_attr[spell_hlf];

			if (cap_col > 0)
			{
			    if (p != prev_ptr
				   && (p - nextline) + cap_col >= nextline_idx)
			    {
				/* Remember that the word in the next line
				 * must start with a capital. */
				capcol_lnum = lnum + 1;
				cap_col = (int)((p - nextline) + cap_col
							       - nextline_idx);
			    }
			    else
				/* Compute the actual column. */
				cap_col += (int)(prev_ptr - line);
			}
		    }
		}
		if (spell_attr != 0)
		{
		    if (!attr_pri)
			char_attr = hl_combine_attr(char_attr, spell_attr);
		    else
			char_attr = hl_combine_attr(spell_attr, char_attr);
		}
#endif
#ifdef FEAT_LINEBREAK
		/*
		 * Found last space before word: check for line break.
		 */
		if (wp->w_p_lbr && vim_isbreak(c) && !vim_isbreak(*ptr))
		{
# ifdef FEAT_MBYTE
		    int mb_off = has_mbyte ? (*mb_head_off)(line, ptr - 1) : 0;
# endif
		    char_u *p = ptr - (
# ifdef FEAT_MBYTE
				mb_off +
# endif
				1);

		    /* TODO: is passing p for start of the line OK? */
		    n_extra = win_lbr_chartabsize(wp, line, p, (colnr_T)vcol,
								    NULL) - 1;
		    if (c == TAB && n_extra + col > W_WIDTH(wp))
			n_extra = (int)wp->w_buffer->b_p_ts
				       - vcol % (int)wp->w_buffer->b_p_ts - 1;

# ifdef FEAT_MBYTE
		    c_extra = mb_off > 0 ? MB_FILLER_CHAR : ' ';
# else
		    c_extra = ' ';
# endif
		    if (vim_iswhite(c))
		    {
#ifdef FEAT_CONCEAL
			if (c == TAB)
			    /* See "Tab alignment" below. */
			    FIX_FOR_BOGUSCOLS;
#endif
			if (!wp->w_p_list)
			    c = ' ';
		    }
		}
#endif

		/* 'list': change char 160 to lcs_nbsp and space to lcs_space.
		 */
		if (wp->w_p_list
			&& (((c == 160
#ifdef FEAT_MBYTE
			      || (mb_utf8 && (mb_c == 160 || mb_c == 0x202f))
#endif
			     ) && lcs_nbsp)
			|| (c == ' ' && lcs_space && ptr - line <= trailcol)))
		{
		    c = (c == ' ') ? lcs_space : lcs_nbsp;
		    if (area_attr == 0 && search_attr == 0)
		    {
			n_attr = 1;
			extra_attr = hl_attr(HLF_8);
			saved_attr2 = char_attr; /* save current attr */
		    }
#ifdef FEAT_MBYTE
		    mb_c = c;
		    if (enc_utf8 && (*mb_char2len)(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    else
			mb_utf8 = FALSE;
#endif
		}

		if (trailcol != MAXCOL && ptr > line + trailcol && c == ' ')
		{
		    c = lcs_trail;
		    if (!attr_pri)
		    {
			n_attr = 1;
			extra_attr = hl_attr(HLF_8);
			saved_attr2 = char_attr; /* save current attr */
		    }
#ifdef FEAT_MBYTE
		    mb_c = c;
		    if (enc_utf8 && (*mb_char2len)(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    else
			mb_utf8 = FALSE;
#endif
		}
	    }

	    /*
	     * Handling of non-printable characters.
	     */
	    if (!vim_isprintc(c))
	    {
		/*
		 * when getting a character from the file, we may have to
		 * turn it into something else on the way to putting it
		 * into "ScreenLines".
		 */
		if (c == TAB && (!wp->w_p_list || lcs_tab1))
		{
		    int tab_len = 0;
		    long vcol_adjusted = vcol; /* removed showbreak length */
#ifdef FEAT_LINEBREAK
		    /* only adjust the tab_len, when at the first column
		     * after the showbreak value was drawn */
		    if (*p_sbr != NUL && vcol == vcol_sbr && wp->w_p_wrap)
			vcol_adjusted = vcol - MB_CHARLEN(p_sbr);
#endif
		    /* tab amount depends on current column */
		    tab_len = (int)wp->w_buffer->b_p_ts
					- vcol_adjusted % (int)wp->w_buffer->b_p_ts - 1;

#ifdef FEAT_LINEBREAK
		    if (!wp->w_p_lbr || !wp->w_p_list)
#endif
		    /* tab amount depends on current column */
			n_extra = tab_len;
#ifdef FEAT_LINEBREAK
		    else
		    {
			char_u *p;
			int	len = n_extra;
			int	i;
			int	saved_nextra = n_extra;

#ifdef FEAT_CONCEAL
			if (vcol_off > 0)
			    /* there are characters to conceal */
			    tab_len += vcol_off;
			/* boguscols before FIX_FOR_BOGUSCOLS macro from above
			 */
			if (wp->w_p_list && lcs_tab1 && old_boguscols > 0
							 && n_extra > tab_len)
			    tab_len += n_extra - tab_len;
#endif

			/* if n_extra > 0, it gives the number of chars, to
			 * use for a tab, else we need to calculate the width
			 * for a tab */
#ifdef FEAT_MBYTE
			len = (tab_len * mb_char2len(lcs_tab2));
			if (n_extra > 0)
			    len += n_extra - tab_len;
#endif
			c = lcs_tab1;
			p = alloc((unsigned)(len + 1));
			vim_memset(p, ' ', len);
			p[len] = NUL;
			p_extra_free = p;
			for (i = 0; i < tab_len; i++)
			{
#ifdef FEAT_MBYTE
			    mb_char2bytes(lcs_tab2, p);
			    p += mb_char2len(lcs_tab2);
			    n_extra += mb_char2len(lcs_tab2)
						 - (saved_nextra > 0 ? 1 : 0);
#else
			    p[i] = lcs_tab2;
#endif
			}
			p_extra = p_extra_free;
#ifdef FEAT_CONCEAL
			/* n_extra will be increased by FIX_FOX_BOGUSCOLS
			 * macro below, so need to adjust for that here */
			if (vcol_off > 0)
			    n_extra -= vcol_off;
#endif
		    }
#endif
#ifdef FEAT_CONCEAL
		    {
			int vc_saved = vcol_off;

			/* Tab alignment should be identical regardless of
			 * 'conceallevel' value. So tab compensates of all
			 * previous concealed characters, and thus resets
			 * vcol_off and boguscols accumulated so far in the
			 * line. Note that the tab can be longer than
			 * 'tabstop' when there are concealed characters. */
			FIX_FOR_BOGUSCOLS;

			/* Make sure, the highlighting for the tab char will be
			 * correctly set further below (effectively reverts the
			 * FIX_FOR_BOGSUCOLS macro */
			if (n_extra == tab_len + vc_saved && wp->w_p_list
								  && lcs_tab1)
			    tab_len += vc_saved;
		    }
#endif
#ifdef FEAT_MBYTE
		    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
		    if (wp->w_p_list)
		    {
			c = lcs_tab1;
#ifdef FEAT_LINEBREAK
			if (wp->w_p_lbr)
			    c_extra = NUL; /* using p_extra from above */
			else
#endif
			    c_extra = lcs_tab2;
			n_attr = tab_len + 1;
			extra_attr = hl_attr(HLF_8);
			saved_attr2 = char_attr; /* save current attr */
#ifdef FEAT_MBYTE
			mb_c = c;
			if (enc_utf8 && (*mb_char2len)(c) > 1)
			{
			    mb_utf8 = TRUE;
			    u8cc[0] = 0;
			    c = 0xc0;
			}
#endif
		    }
		    else
		    {
			c_extra = ' ';
			c = ' ';
		    }
		}
		else if (c == NUL
			&& (wp->w_p_list
			    || ((fromcol >= 0 || fromcol_prev >= 0)
				&& tocol > vcol
				&& VIsual_mode != Ctrl_V
				&& (
# ifdef FEAT_RIGHTLEFT
				    wp->w_p_rl ? (col >= 0) :
# endif
				    (col < W_WIDTH(wp)))
				&& !(noinvcur
				    && lnum == wp->w_cursor.lnum
				    && (colnr_T)vcol == wp->w_virtcol)))
			&& lcs_eol_one > 0)
		{
		    /* Display a '$' after the line or highlight an extra
		     * character if the line break is included. */
#if defined(FEAT_DIFF) || defined(LINE_ATTR)
		    /* For a diff line the highlighting continues after the
		     * "$". */
		    if (
# ifdef FEAT_DIFF
			    diff_hlf == (hlf_T)0
#  ifdef LINE_ATTR
			    &&
#  endif
# endif
# ifdef LINE_ATTR
			    line_attr == 0
# endif
		       )
#endif
		    {
#ifdef FEAT_VIRTUALEDIT
			/* In virtualedit, visual selections may extend
			 * beyond end of line. */
			if (area_highlighting && virtual_active()
				&& tocol != MAXCOL && vcol < tocol)
			    n_extra = 0;
			else
#endif
			{
			    p_extra = at_end_str;
			    n_extra = 1;
			    c_extra = NUL;
			}
		    }
		    if (wp->w_p_list && lcs_eol > 0)
			c = lcs_eol;
		    else
			c = ' ';
		    lcs_eol_one = -1;
		    --ptr;	    /* put it back at the NUL */
		    if (!attr_pri)
		    {
			extra_attr = hl_attr(HLF_AT);
			n_attr = 1;
		    }
#ifdef FEAT_MBYTE
		    mb_c = c;
		    if (enc_utf8 && (*mb_char2len)(c) > 1)
		    {
			mb_utf8 = TRUE;
			u8cc[0] = 0;
			c = 0xc0;
		    }
		    else
			mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
		}
		else if (c != NUL)
		{
		    p_extra = transchar(c);
		    if (n_extra == 0)
			n_extra = byte2cells(c) - 1;
#ifdef FEAT_RIGHTLEFT
		    if ((dy_flags & DY_UHEX) && wp->w_p_rl)
			rl_mirror(p_extra);	/* reverse "<12>" */
#endif
		    c_extra = NUL;
#ifdef FEAT_LINEBREAK
		    if (wp->w_p_lbr)
		    {
			char_u *p;

			c = *p_extra;
			p = alloc((unsigned)n_extra + 1);
			vim_memset(p, ' ', n_extra);
			STRNCPY(p, p_extra + 1, STRLEN(p_extra) - 1);
			p[n_extra] = NUL;
			p_extra_free = p_extra = p;
		    }
		    else
#endif
		    {
			n_extra = byte2cells(c) - 1;
			c = *p_extra++;
		    }
		    if (!attr_pri)
		    {
			n_attr = n_extra + 1;
			extra_attr = hl_attr(HLF_8);
			saved_attr2 = char_attr; /* save current attr */
		    }
#ifdef FEAT_MBYTE
		    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
		}
#ifdef FEAT_VIRTUALEDIT
		else if (VIsual_active
			 && (VIsual_mode == Ctrl_V
			     || VIsual_mode == 'v')
			 && virtual_active()
			 && tocol != MAXCOL
			 && vcol < tocol
			 && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (col >= 0) :
# endif
			    (col < W_WIDTH(wp))))
		{
		    c = ' ';
		    --ptr;	    /* put it back at the NUL */
		}
#endif
#if defined(LINE_ATTR)
		else if ((
# ifdef FEAT_DIFF
			    diff_hlf != (hlf_T)0 ||
# endif
			    line_attr != 0
			) && (
# ifdef FEAT_RIGHTLEFT
			    wp->w_p_rl ? (col >= 0) :
# endif
			    (col
# ifdef FEAT_CONCEAL
				- boguscols
# endif
					    < W_WIDTH(wp))))
		{
		    /* Highlight until the right side of the window */
		    c = ' ';
		    --ptr;	    /* put it back at the NUL */

		    /* Remember we do the char for line highlighting. */
		    ++did_line_attr;

		    /* don't do search HL for the rest of the line */
		    if (line_attr != 0 && char_attr == search_attr && col > 0)
			char_attr = line_attr;
# ifdef FEAT_DIFF
		    if (diff_hlf == HLF_TXD)
		    {
			diff_hlf = HLF_CHD;
			if (attr == 0 || char_attr != attr)
			{
			    char_attr = hl_attr(diff_hlf);
			    if (wp->w_p_cul && lnum == wp->w_cursor.lnum)
				char_attr = hl_combine_attr(char_attr,
							    hl_attr(HLF_CUL));
			}
		    }
# endif
		}
#endif
	    }

#ifdef FEAT_CONCEAL
	    if (   wp->w_p_cole > 0
		&& (wp != curwin || lnum != wp->w_cursor.lnum ||
							conceal_cursor_line(wp) )
		&& ( (syntax_flags & HL_CONCEAL) != 0 || has_match_conc)
		&& !(lnum_in_visual_area
				    && vim_strchr(wp->w_p_cocu, 'v') == NULL))
	    {
		char_attr = conceal_attr;
		if (prev_syntax_id != syntax_seqnr
			&& (syn_get_sub_char() != NUL || match_conc
							 || wp->w_p_cole == 1)
			&& wp->w_p_cole != 3)
		{
		    /* First time at this concealed item: display one
		     * character. */
		    if (match_conc)
			c = match_conc;
		    else if (syn_get_sub_char() != NUL)
			c = syn_get_sub_char();
		    else if (lcs_conceal != NUL)
			c = lcs_conceal;
		    else
			c = ' ';

		    prev_syntax_id = syntax_seqnr;

		    if (n_extra > 0)
			vcol_off += n_extra;
		    vcol += n_extra;
		    if (wp->w_p_wrap && n_extra > 0)
		    {
# ifdef FEAT_RIGHTLEFT
			if (wp->w_p_rl)
			{
			    col -= n_extra;
			    boguscols -= n_extra;
			}
			else
# endif
			{
			    boguscols += n_extra;
			    col += n_extra;
			}
		    }
		    n_extra = 0;
		    n_attr = 0;
		}
		else if (n_skip == 0)
		{
		    is_concealing = TRUE;
		    n_skip = 1;
		}
# ifdef FEAT_MBYTE
		mb_c = c;
		if (enc_utf8 && (*mb_char2len)(c) > 1)
		{
		    mb_utf8 = TRUE;
		    u8cc[0] = 0;
		    c = 0xc0;
		}
		else
		    mb_utf8 = FALSE;	/* don't draw as UTF-8 */
# endif
	    }
	    else
	    {
		prev_syntax_id = 0;
		is_concealing = FALSE;
	    }
#endif /* FEAT_CONCEAL */
	}

#ifdef FEAT_CONCEAL
	/* In the cursor line and we may be concealing characters: correct
	 * the cursor column when we reach its position. */
	if (!did_wcol && draw_state == WL_LINE
		&& wp == curwin && lnum == wp->w_cursor.lnum
		&& conceal_cursor_line(wp)
		&& (int)wp->w_virtcol <= vcol + n_skip)
	{
#  ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
		wp->w_wcol = W_WIDTH(wp) - col + boguscols - 1;
	    else
#  endif
		wp->w_wcol = col - boguscols;
	    wp->w_wrow = row;
	    did_wcol = TRUE;
	}
#endif

	/* Don't override visual selection highlighting. */
	if (n_attr > 0
		&& draw_state == WL_LINE
		&& !attr_pri)
	    char_attr = extra_attr;

#if defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
	/* XIM don't send preedit_start and preedit_end, but they send
	 * preedit_changed and commit.  Thus Vim can't set "im_is_active", use
	 * im_is_preediting() here. */
	if (xic != NULL
		&& lnum == wp->w_cursor.lnum
		&& (State & INSERT)
		&& !p_imdisable
		&& im_is_preediting()
		&& draw_state == WL_LINE)
	{
	    colnr_T tcol;

	    if (preedit_end_col == MAXCOL)
		getvcol(curwin, &(wp->w_cursor), &tcol, NULL, NULL);
	    else
		tcol = preedit_end_col;
	    if ((long)preedit_start_col <= vcol && vcol < (long)tcol)
	    {
		if (feedback_old_attr < 0)
		{
		    feedback_col = 0;
		    feedback_old_attr = char_attr;
		}
		char_attr = im_get_feedback_attr(feedback_col);
		if (char_attr < 0)
		    char_attr = feedback_old_attr;
		feedback_col++;
	    }
	    else if (feedback_old_attr >= 0)
	    {
		char_attr = feedback_old_attr;
		feedback_old_attr = -1;
		feedback_col = 0;
	    }
	}
#endif
	/*
	 * Handle the case where we are in column 0 but not on the first
	 * character of the line and the user wants us to show us a
	 * special character (via 'listchars' option "precedes:<char>".
	 */
	if (lcs_prec_todo != NUL
		&& wp->w_p_list
		&& (wp->w_p_wrap ? wp->w_skipcol > 0 : wp->w_leftcol > 0)
#ifdef FEAT_DIFF
		&& filler_todo <= 0
#endif
		&& draw_state > WL_NR
		&& c != NUL)
	{
	    c = lcs_prec;
	    lcs_prec_todo = NUL;
#ifdef FEAT_MBYTE
	    if (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		/* Double-width character being overwritten by the "precedes"
		 * character, need to fill up half the character. */
		c_extra = MB_FILLER_CHAR;
		n_extra = 1;
		n_attr = 2;
		extra_attr = hl_attr(HLF_AT);
	    }
	    mb_c = c;
	    if (enc_utf8 && (*mb_char2len)(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    else
		mb_utf8 = FALSE;	/* don't draw as UTF-8 */
#endif
	    if (!attr_pri)
	    {
		saved_attr3 = char_attr; /* save current attr */
		char_attr = hl_attr(HLF_AT); /* later copied to char_attr */
		n_attr3 = 1;
	    }
	}

	/*
	 * At end of the text line or just after the last character.
	 */
	if (c == NUL
#if defined(LINE_ATTR)
		|| did_line_attr == 1
#endif
		)
	{
#ifdef FEAT_SEARCH_EXTRA
	    long prevcol = (long)(ptr - line) - (c == NUL);

	    /* we're not really at that column when skipping some text */
	    if ((long)(wp->w_p_wrap ? wp->w_skipcol : wp->w_leftcol) > prevcol)
		++prevcol;
#endif

	    /* Invert at least one char, used for Visual and empty line or
	     * highlight match at end of line. If it's beyond the last
	     * char on the screen, just overwrite that one (tricky!)  Not
	     * needed when a '$' was displayed for 'list'. */
#ifdef FEAT_SEARCH_EXTRA
	    prevcol_hl_flag = FALSE;
	    if (prevcol == (long)search_hl.startcol)
		prevcol_hl_flag = TRUE;
	    else
	    {
		cur = wp->w_match_head;
		while (cur != NULL)
		{
		    if (prevcol == (long)cur->hl.startcol)
		    {
			prevcol_hl_flag = TRUE;
			break;
		    }
		    cur = cur->next;
		}
	    }
#endif
	    if (lcs_eol == lcs_eol_one
		    && ((area_attr != 0 && vcol == fromcol
			    && (VIsual_mode != Ctrl_V
				|| lnum == VIsual.lnum
				|| lnum == curwin->w_cursor.lnum)
			    && c == NUL)
#ifdef FEAT_SEARCH_EXTRA
			/* highlight 'hlsearch' match at end of line */
			|| (prevcol_hl_flag == TRUE
# if defined(LINE_ATTR)
			    && did_line_attr <= 1
# endif
			   )
#endif
		       ))
	    {
		int n = 0;

#ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		{
		    if (col < 0)
			n = 1;
		}
		else
#endif
		{
		    if (col >= W_WIDTH(wp))
			n = -1;
		}
		if (n != 0)
		{
		    /* At the window boundary, highlight the last character
		     * instead (better than nothing). */
		    off += n;
		    col += n;
		}
		else
		{
		    /* Add a blank character to highlight. */
		    ScreenLines[off] = ' ';
#ifdef FEAT_MBYTE
		    if (enc_utf8)
			ScreenLinesUC[off] = 0;
#endif
		}
#ifdef FEAT_SEARCH_EXTRA
		if (area_attr == 0)
		{
		    /* Use attributes from match with highest priority among
		     * 'search_hl' and the match list. */
		    char_attr = search_hl.attr;
		    cur = wp->w_match_head;
		    shl_flag = FALSE;
		    while (cur != NULL || shl_flag == FALSE)
		    {
			if (shl_flag == FALSE
				&& ((cur != NULL
					&& cur->priority > SEARCH_HL_PRIORITY)
				    || cur == NULL))
			{
			    shl = &search_hl;
			    shl_flag = TRUE;
			}
			else
			    shl = &cur->hl;
			if ((ptr - line) - 1 == (long)shl->startcol)
			    char_attr = shl->attr;
			if (shl != &search_hl && cur != NULL)
			    cur = cur->next;
		    }
		}
#endif
		ScreenAttrs[off] = char_attr;
#ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		{
		    --col;
		    --off;
		}
		else
#endif
		{
		    ++col;
		    ++off;
		}
		++vcol;
#ifdef FEAT_SYN_HL
		eol_hl_off = 1;
#endif
	    }
	}

	/*
	 * At end of the text line.
	 */
	if (c == NUL)
	{
#ifdef FEAT_SYN_HL
	    if (eol_hl_off > 0 && vcol - eol_hl_off == (long)wp->w_virtcol
		    && lnum == wp->w_cursor.lnum)
	    {
		/* highlight last char after line */
		--col;
		--off;
		--vcol;
	    }

	    /* Highlight 'cursorcolumn' & 'colorcolumn' past end of the line. */
	    if (wp->w_p_wrap)
		v = wp->w_skipcol;
	    else
		v = wp->w_leftcol;

	    /* check if line ends before left margin */
	    if (vcol < v + col - win_col_off(wp))
		vcol = v + col - win_col_off(wp);
#ifdef FEAT_CONCEAL
	    /* Get rid of the boguscols now, we want to draw until the right
	     * edge for 'cursorcolumn'. */
	    col -= boguscols;
	    boguscols = 0;
#endif

	    if (draw_color_col)
		draw_color_col = advance_color_col(VCOL_HLC, &color_cols);

	    if (((wp->w_p_cuc
		      && (int)wp->w_virtcol >= VCOL_HLC - eol_hl_off
		      && (int)wp->w_virtcol <
					W_WIDTH(wp) * (row - startrow + 1) + v
		      && lnum != wp->w_cursor.lnum)
		    || draw_color_col)
# ifdef FEAT_RIGHTLEFT
		    && !wp->w_p_rl
# endif
		    )
	    {
		int	rightmost_vcol = 0;
		int	i;

		if (wp->w_p_cuc)
		    rightmost_vcol = wp->w_virtcol;
		if (draw_color_col)
		    /* determine rightmost colorcolumn to possibly draw */
		    for (i = 0; color_cols[i] >= 0; ++i)
			if (rightmost_vcol < color_cols[i])
			    rightmost_vcol = color_cols[i];

		while (col < W_WIDTH(wp))
		{
		    ScreenLines[off] = ' ';
#ifdef FEAT_MBYTE
		    if (enc_utf8)
			ScreenLinesUC[off] = 0;
#endif
		    ++col;
		    if (draw_color_col)
			draw_color_col = advance_color_col(VCOL_HLC,
								 &color_cols);

		    if (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol)
			ScreenAttrs[off++] = hl_attr(HLF_CUC);
		    else if (draw_color_col && VCOL_HLC == *color_cols)
			ScreenAttrs[off++] = hl_attr(HLF_MC);
		    else
			ScreenAttrs[off++] = 0;

		    if (VCOL_HLC >= rightmost_vcol)
			break;

		    ++vcol;
		}
	    }
#endif

	    SCREEN_LINE(screen_row, W_WINCOL(wp), col,
						(int)W_WIDTH(wp), wp->w_p_rl);
	    row++;

	    /*
	     * Update w_cline_height and w_cline_folded if the cursor line was
	     * updated (saves a call to plines() later).
	     */
	    if (wp == curwin && lnum == curwin->w_cursor.lnum)
	    {
		curwin->w_cline_row = startrow;
		curwin->w_cline_height = row - startrow;
#ifdef FEAT_FOLDING
		curwin->w_cline_folded = FALSE;
#endif
		curwin->w_valid |= (VALID_CHEIGHT|VALID_CROW);
	    }

	    break;
	}

	/* line continues beyond line end */
	if (lcs_ext
		&& !wp->w_p_wrap
#ifdef FEAT_DIFF
		&& filler_todo <= 0
#endif
		&& (
#ifdef FEAT_RIGHTLEFT
		    wp->w_p_rl ? col == 0 :
#endif
		    col == W_WIDTH(wp) - 1)
		&& (*ptr != NUL
		    || (wp->w_p_list && lcs_eol_one > 0)
		    || (n_extra && (c_extra != NUL || *p_extra != NUL))))
	{
	    c = lcs_ext;
	    char_attr = hl_attr(HLF_AT);
#ifdef FEAT_MBYTE
	    mb_c = c;
	    if (enc_utf8 && (*mb_char2len)(c) > 1)
	    {
		mb_utf8 = TRUE;
		u8cc[0] = 0;
		c = 0xc0;
	    }
	    else
		mb_utf8 = FALSE;
#endif
	}

#ifdef FEAT_SYN_HL
	/* advance to the next 'colorcolumn' */
	if (draw_color_col)
	    draw_color_col = advance_color_col(VCOL_HLC, &color_cols);

	/* Highlight the cursor column if 'cursorcolumn' is set.  But don't
	 * highlight the cursor position itself.
	 * Also highlight the 'colorcolumn' if it is different than
	 * 'cursorcolumn' */
	vcol_save_attr = -1;
	if (draw_state == WL_LINE && !lnum_in_visual_area)
	{
	    if (wp->w_p_cuc && VCOL_HLC == (long)wp->w_virtcol
						 && lnum != wp->w_cursor.lnum)
	    {
		vcol_save_attr = char_attr;
		char_attr = hl_combine_attr(char_attr, hl_attr(HLF_CUC));
	    }
	    else if (draw_color_col && VCOL_HLC == *color_cols)
	    {
		vcol_save_attr = char_attr;
		char_attr = hl_combine_attr(char_attr, hl_attr(HLF_MC));
	    }
	}
#endif

	/*
	 * Store character to be displayed.
	 * Skip characters that are left of the screen for 'nowrap'.
	 */
	vcol_prev = vcol;
	if (draw_state < WL_LINE || n_skip <= 0)
	{
	    /*
	     * Store the character.
	     */
#if defined(FEAT_RIGHTLEFT) && defined(FEAT_MBYTE)
	    if (has_mbyte && wp->w_p_rl && (*mb_char2cells)(mb_c) > 1)
	    {
		/* A double-wide character is: put first halve in left cell. */
		--off;
		--col;
	    }
#endif
	    ScreenLines[off] = c;
#ifdef FEAT_MBYTE
	    if (enc_dbcs == DBCS_JPNU)
	    {
		if ((mb_c & 0xff00) == 0x8e00)
		    ScreenLines[off] = 0x8e;
		ScreenLines2[off] = mb_c & 0xff;
	    }
	    else if (enc_utf8)
	    {
		if (mb_utf8)
		{
		    int i;

		    ScreenLinesUC[off] = mb_c;
		    if ((c & 0xff) == 0)
			ScreenLines[off] = 0x80;   /* avoid storing zero */
		    for (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][off] = u8cc[i];
			if (u8cc[i] == 0)
			    break;
		    }
		}
		else
		    ScreenLinesUC[off] = 0;
	    }
	    if (multi_attr)
	    {
		ScreenAttrs[off] = multi_attr;
		multi_attr = 0;
	    }
	    else
#endif
		ScreenAttrs[off] = char_attr;

#ifdef FEAT_MBYTE
	    if (has_mbyte && (*mb_char2cells)(mb_c) > 1)
	    {
		/* Need to fill two screen columns. */
		++off;
		++col;
		if (enc_utf8)
		    /* UTF-8: Put a 0 in the second screen char. */
		    ScreenLines[off] = 0;
		else
		    /* DBCS: Put second byte in the second screen char. */
		    ScreenLines[off] = mb_c & 0xff;
		if (draw_state > WL_NR
#ifdef FEAT_DIFF
			&& filler_todo <= 0
#endif
			)
		    ++vcol;
		/* When "tocol" is halfway a character, set it to the end of
		 * the character, otherwise highlighting won't stop. */
		if (tocol == vcol)
		    ++tocol;
#ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		{
		    /* now it's time to backup one cell */
		    --off;
		    --col;
		}
#endif
	    }
#endif
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
	    {
		--off;
		--col;
	    }
	    else
#endif
	    {
		++off;
		++col;
	    }
	}
#ifdef FEAT_CONCEAL
	else if (wp->w_p_cole > 0 && is_concealing)
	{
	    --n_skip;
	    ++vcol_off;
	    if (n_extra > 0)
		vcol_off += n_extra;
	    if (wp->w_p_wrap)
	    {
		/*
		 * Special voodoo required if 'wrap' is on.
		 *
		 * Advance the column indicator to force the line
		 * drawing to wrap early. This will make the line
		 * take up the same screen space when parts are concealed,
		 * so that cursor line computations aren't messed up.
		 *
		 * To avoid the fictitious advance of 'col' causing
		 * trailing junk to be written out of the screen line
		 * we are building, 'boguscols' keeps track of the number
		 * of bad columns we have advanced.
		 */
		if (n_extra > 0)
		{
		    vcol += n_extra;
# ifdef FEAT_RIGHTLEFT
		    if (wp->w_p_rl)
		    {
			col -= n_extra;
			boguscols -= n_extra;
		    }
		    else
# endif
		    {
			col += n_extra;
			boguscols += n_extra;
		    }
		    n_extra = 0;
		    n_attr = 0;
		}


# ifdef FEAT_MBYTE
		if (has_mbyte && (*mb_char2cells)(mb_c) > 1)
		{
		    /* Need to fill two screen columns. */
#  ifdef FEAT_RIGHTLEFT
		    if (wp->w_p_rl)
		    {
			--boguscols;
			--col;
		    }
		    else
#  endif
		    {
			++boguscols;
			++col;
		    }
		}
# endif

# ifdef FEAT_RIGHTLEFT
		if (wp->w_p_rl)
		{
		    --boguscols;
		    --col;
		}
		else
# endif
		{
		    ++boguscols;
		    ++col;
		}
	    }
	    else
	    {
		if (n_extra > 0)
		{
		    vcol += n_extra;
		    n_extra = 0;
		    n_attr = 0;
		}
	    }

	}
#endif /* FEAT_CONCEAL */
	else
	    --n_skip;

	/* Only advance the "vcol" when after the 'number' or 'relativenumber'
	 * column. */
	if (draw_state > WL_NR
#ifdef FEAT_DIFF
		&& filler_todo <= 0
#endif
		)
	    ++vcol;

#ifdef FEAT_SYN_HL
	if (vcol_save_attr >= 0)
	    char_attr = vcol_save_attr;
#endif

	/* restore attributes after "predeces" in 'listchars' */
	if (draw_state > WL_NR && n_attr3 > 0 && --n_attr3 == 0)
	    char_attr = saved_attr3;

	/* restore attributes after last 'listchars' or 'number' char */
	if (n_attr > 0 && draw_state == WL_LINE && --n_attr == 0)
	    char_attr = saved_attr2;

	/*
	 * At end of screen line and there is more to come: Display the line
	 * so far.  If there is no more to display it is caught above.
	 */
	if ((
#ifdef FEAT_RIGHTLEFT
	    wp->w_p_rl ? (col < 0) :
#endif
				    (col >= W_WIDTH(wp)))
		&& (*ptr != NUL
#ifdef FEAT_DIFF
		    || filler_todo > 0
#endif
		    || (wp->w_p_list && lcs_eol != NUL && p_extra != at_end_str)
		    || (n_extra != 0 && (c_extra != NUL || *p_extra != NUL)))
		)
	{
#ifdef FEAT_CONCEAL
	    SCREEN_LINE(screen_row, W_WINCOL(wp), col - boguscols,
						(int)W_WIDTH(wp), wp->w_p_rl);
	    boguscols = 0;
#else
	    SCREEN_LINE(screen_row, W_WINCOL(wp), col,
						(int)W_WIDTH(wp), wp->w_p_rl);
#endif
	    ++row;
	    ++screen_row;

	    /* When not wrapping and finished diff lines, or when displayed
	     * '$' and highlighting until last column, break here. */
	    if ((!wp->w_p_wrap
#ifdef FEAT_DIFF
		    && filler_todo <= 0
#endif
		    ) || lcs_eol_one == -1)
		break;

	    /* When the window is too narrow draw all "@" lines. */
	    if (draw_state != WL_LINE
#ifdef FEAT_DIFF
		    && filler_todo <= 0
#endif
		    )
	    {
		win_draw_end(wp, '@', ' ', row, wp->w_height, HLF_AT);
#ifdef FEAT_WINDOWS
		draw_vsep_win(wp, row);
#endif
		row = endrow;
	    }

	    /* When line got too long for screen break here. */
	    if (row == endrow)
	    {
		++row;
		break;
	    }

	    if (screen_cur_row == screen_row - 1
#ifdef FEAT_DIFF
		     && filler_todo <= 0
#endif
		     && W_WIDTH(wp) == Columns)
	    {
		/* Remember that the line wraps, used for modeless copy. */
		LineWraps[screen_row - 1] = TRUE;

		/*
		 * Special trick to make copy/paste of wrapped lines work with
		 * xterm/screen: write an extra character beyond the end of
		 * the line. This will work with all terminal types
		 * (regardless of the xn,am settings).
		 * Only do this on a fast tty.
		 * Only do this if the cursor is on the current line
		 * (something has been written in it).
		 * Don't do this for the GUI.
		 * Don't do this for double-width characters.
		 * Don't do this for a window not at the right screen border.
		 */
		if (p_tf
#ifdef FEAT_GUI
			 && !gui.in_use
#endif
#ifdef FEAT_MBYTE
			 && !(has_mbyte
			     && ((*mb_off2cells)(LineOffset[screen_row],
				     LineOffset[screen_row] + screen_Columns)
									  == 2
				 || (*mb_off2cells)(LineOffset[screen_row - 1]
							+ (int)Columns - 2,
				     LineOffset[screen_row] + screen_Columns)
									== 2))
#endif
		   )
		{
		    /* First make sure we are at the end of the screen line,
		     * then output the same character again to let the
		     * terminal know about the wrap.  If the terminal doesn't
		     * auto-wrap, we overwrite the character. */
		    if (screen_cur_col != W_WIDTH(wp))
			screen_char(LineOffset[screen_row - 1]
						      + (unsigned)Columns - 1,
					  screen_row - 1, (int)(Columns - 1));

#ifdef FEAT_MBYTE
		    /* When there is a multi-byte character, just output a
		     * space to keep it simple. */
		    if (has_mbyte && MB_BYTE2LEN(ScreenLines[LineOffset[
					screen_row - 1] + (Columns - 1)]) > 1)
			out_char(' ');
		    else
#endif
			out_char(ScreenLines[LineOffset[screen_row - 1]
							    + (Columns - 1)]);
		    /* force a redraw of the first char on the next line */
		    ScreenAttrs[LineOffset[screen_row]] = (sattr_T)-1;
		    screen_start();	/* don't know where cursor is now */
		}
	    }

	    col = 0;
	    off = (unsigned)(current_ScreenLine - ScreenLines);
#ifdef FEAT_RIGHTLEFT
	    if (wp->w_p_rl)
	    {
		col = W_WIDTH(wp) - 1;	/* col is not used if breaking! */
		off += col;
	    }
#endif

	    /* reset the drawing state for the start of a wrapped line */
	    draw_state = WL_START;
	    saved_n_extra = n_extra;
	    saved_p_extra = p_extra;
	    saved_c_extra = c_extra;
	    saved_char_attr = char_attr;
	    n_extra = 0;
	    lcs_prec_todo = lcs_prec;
#ifdef FEAT_LINEBREAK
# ifdef FEAT_DIFF
	    if (filler_todo <= 0)
# endif
		need_showbreak = TRUE;
#endif
#ifdef FEAT_DIFF
	    --filler_todo;
	    /* When the filler lines are actually below the last line of the
	     * file, don't draw the line itself, break here. */
	    if (filler_todo == 0 && wp->w_botfill)
		break;
#endif
	}

    }	/* for every character in the line */

#ifdef FEAT_SPELL
    /* After an empty line check first word for capital. */
    if (*skipwhite(line) == NUL)
    {
	capcol_lnum = lnum + 1;
	cap_col = 0;
    }
#endif

    return row;
}

#ifdef FEAT_MBYTE
static int comp_char_differs(int, int);

/*
 * Return if the composing characters at "off_from" and "off_to" differ.
 * Only to be used when ScreenLinesUC[off_from] != 0.
 */
    static int
comp_char_differs(int off_from, int off_to)
{
    int	    i;

    for (i = 0; i < Screen_mco; ++i)
    {
	if (ScreenLinesC[i][off_from] != ScreenLinesC[i][off_to])
	    return TRUE;
	if (ScreenLinesC[i][off_from] == 0)
	    break;
    }
    return FALSE;
}
#endif

/*
 * Check whether the given character needs redrawing:
 * - the (first byte of the) character is different
 * - the attributes are different
 * - the character is multi-byte and the next byte is different
 * - the character is two cells wide and the second cell differs.
 */
    static int
char_needs_redraw(int off_from, int off_to, int cols)
{
    if (cols > 0
	    && ((ScreenLines[off_from] != ScreenLines[off_to]
		    || ScreenAttrs[off_from] != ScreenAttrs[off_to])

#ifdef FEAT_MBYTE
		|| (enc_dbcs != 0
		    && MB_BYTE2LEN(ScreenLines[off_from]) > 1
		    && (enc_dbcs == DBCS_JPNU && ScreenLines[off_from] == 0x8e
			? ScreenLines2[off_from] != ScreenLines2[off_to]
			: (cols > 1 && ScreenLines[off_from + 1]
						 != ScreenLines[off_to + 1])))
		|| (enc_utf8
		    && (ScreenLinesUC[off_from] != ScreenLinesUC[off_to]
			|| (ScreenLinesUC[off_from] != 0
			    && comp_char_differs(off_from, off_to))
			|| ((*mb_off2cells)(off_from, off_from + cols) > 1
			    && ScreenLines[off_from + 1]
						  != ScreenLines[off_to + 1])))
#endif
	       ))
	return TRUE;
    return FALSE;
}

/*
 * Move one "cooked" screen line to the screen, but only the characters that
 * have actually changed.  Handle insert/delete character.
 * "coloff" gives the first column on the screen for this line.
 * "endcol" gives the columns where valid characters are.
 * "clear_width" is the width of the window.  It's > 0 if the rest of the line
 * needs to be cleared, negative otherwise.
 * "rlflag" is TRUE in a rightleft window:
 *    When TRUE and "clear_width" > 0, clear columns 0 to "endcol"
 *    When FALSE and "clear_width" > 0, clear columns "endcol" to "clear_width"
 */
    static void
screen_line(
    int	    row,
    int	    coloff,
    int	    endcol,
    int	    clear_width
#ifdef FEAT_RIGHTLEFT
    , int   rlflag
#endif
    )
{
    unsigned	    off_from;
    unsigned	    off_to;
#ifdef FEAT_MBYTE
    unsigned	    max_off_from;
    unsigned	    max_off_to;
#endif
    int		    col = 0;
#if defined(FEAT_GUI) || defined(UNIX) || defined(FEAT_WINDOWS)
    int		    hl;
#endif
    int		    force = FALSE;	/* force update rest of the line */
    int		    redraw_this		/* bool: does character need redraw? */
#ifdef FEAT_GUI
				= TRUE	/* For GUI when while-loop empty */
#endif
				;
    int		    redraw_next;	/* redraw_this for next character */
#ifdef FEAT_MBYTE
    int		    clear_next = FALSE;
    int		    char_cells;		/* 1: normal char */
					/* 2: occupies two display cells */
# define CHAR_CELLS char_cells
#else
# define CHAR_CELLS 1
#endif

    /* Check for illegal row and col, just in case. */
    if (row >= Rows)
	row = Rows - 1;
    if (endcol > Columns)
	endcol = Columns;

# ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, row);
# endif

    off_from = (unsigned)(current_ScreenLine - ScreenLines);
    off_to = LineOffset[row] + coloff;
#ifdef FEAT_MBYTE
    max_off_from = off_from + screen_Columns;
    max_off_to = LineOffset[row] + screen_Columns;
#endif

#ifdef FEAT_RIGHTLEFT
    if (rlflag)
    {
	/* Clear rest first, because it's left of the text. */
	if (clear_width > 0)
	{
	    while (col <= endcol && ScreenLines[off_to] == ' '
		    && ScreenAttrs[off_to] == 0
# ifdef FEAT_MBYTE
				  && (!enc_utf8 || ScreenLinesUC[off_to] == 0)
# endif
						  )
	    {
		++off_to;
		++col;
	    }
	    if (col <= endcol)
		screen_fill(row, row + 1, col + coloff,
					    endcol + coloff + 1, ' ', ' ', 0);
	}
	col = endcol + 1;
	off_to = LineOffset[row] + col + coloff;
	off_from += col;
	endcol = (clear_width > 0 ? clear_width : -clear_width);
    }
#endif /* FEAT_RIGHTLEFT */

    redraw_next = char_needs_redraw(off_from, off_to, endcol - col);

    while (col < endcol)
    {
#ifdef FEAT_MBYTE
	if (has_mbyte && (col + 1 < endcol))
	    char_cells = (*mb_off2cells)(off_from, max_off_from);
	else
	    char_cells = 1;
#endif

	redraw_this = redraw_next;
	redraw_next = force || char_needs_redraw(off_from + CHAR_CELLS,
			      off_to + CHAR_CELLS, endcol - col - CHAR_CELLS);

#ifdef FEAT_GUI
	/* If the next character was bold, then redraw the current character to
	 * remove any pixels that might have spilt over into us.  This only
	 * happens in the GUI.
	 */
	if (redraw_next && gui.in_use)
	{
	    hl = ScreenAttrs[off_to + CHAR_CELLS];
	    if (hl > HL_ALL)
		hl = syn_attr2attr(hl);
	    if (hl & HL_BOLD)
		redraw_this = TRUE;
	}
#endif

	if (redraw_this)
	{
	    /*
	     * Special handling when 'xs' termcap flag set (hpterm):
	     * Attributes for characters are stored at the position where the
	     * cursor is when writing the highlighting code.  The
	     * start-highlighting code must be written with the cursor on the
	     * first highlighted character.  The stop-highlighting code must
	     * be written with the cursor just after the last highlighted
	     * character.
	     * Overwriting a character doesn't remove it's highlighting.  Need
	     * to clear the rest of the line, and force redrawing it
	     * completely.
	     */
	    if (       p_wiv
		    && !force
#ifdef FEAT_GUI
		    && !gui.in_use
#endif
		    && ScreenAttrs[off_to] != 0
		    && ScreenAttrs[off_from] != ScreenAttrs[off_to])
	    {
		/*
		 * Need to remove highlighting attributes here.
		 */
		windgoto(row, col + coloff);
		out_str(T_CE);		/* clear rest of this screen line */
		screen_start();		/* don't know where cursor is now */
		force = TRUE;		/* force redraw of rest of the line */
		redraw_next = TRUE;	/* or else next char would miss out */

		/*
		 * If the previous character was highlighted, need to stop
		 * highlighting at this character.
		 */
		if (col + coloff > 0 && ScreenAttrs[off_to - 1] != 0)
		{
		    screen_attr = ScreenAttrs[off_to - 1];
		    term_windgoto(row, col + coloff);
		    screen_stop_highlight();
		}
		else
		    screen_attr = 0;	    /* highlighting has stopped */
	    }
#ifdef FEAT_MBYTE
	    if (enc_dbcs != 0)
	    {
		/* Check if overwriting a double-byte with a single-byte or
		 * the other way around requires another character to be
		 * redrawn.  For UTF-8 this isn't needed, because comparing
		 * ScreenLinesUC[] is sufficient. */
		if (char_cells == 1
			&& col + 1 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) > 1)
		{
		    /* Writing a single-cell character over a double-cell
		     * character: need to redraw the next cell. */
		    ScreenLines[off_to + 1] = 0;
		    redraw_next = TRUE;
		}
		else if (char_cells == 2
			&& col + 2 < endcol
			&& (*mb_off2cells)(off_to, max_off_to) == 1
			&& (*mb_off2cells)(off_to + 1, max_off_to) > 1)
		{
		    /* Writing the second half of a double-cell character over
		     * a double-cell character: need to redraw the second
		     * cell. */
		    ScreenLines[off_to + 2] = 0;
		    redraw_next = TRUE;
		}

		if (enc_dbcs == DBCS_JPNU)
		    ScreenLines2[off_to] = ScreenLines2[off_from];
	    }
	    /* When writing a single-width character over a double-width
	     * character and at the end of the redrawn text, need to clear out
	     * the right halve of the old character.
	     * Also required when writing the right halve of a double-width
	     * char over the left halve of an existing one. */
	    if (has_mbyte && col + char_cells == endcol
		    && ((char_cells == 1
			    && (*mb_off2cells)(off_to, max_off_to) > 1)
			|| (char_cells == 2
			    && (*mb_off2cells)(off_to, max_off_to) == 1
			    && (*mb_off2cells)(off_to + 1, max_off_to) > 1)))
		clear_next = TRUE;
#endif

	    ScreenLines[off_to] = ScreenLines[off_from];
#ifdef FEAT_MBYTE
	    if (enc_utf8)
	    {
		ScreenLinesUC[off_to] = ScreenLinesUC[off_from];
		if (ScreenLinesUC[off_from] != 0)
		{
		    int	    i;

		    for (i = 0; i < Screen_mco; ++i)
			ScreenLinesC[i][off_to] = ScreenLinesC[i][off_from];
		}
	    }
	    if (char_cells == 2)
		ScreenLines[off_to + 1] = ScreenLines[off_from + 1];
#endif

#if defined(FEAT_GUI) || defined(UNIX)
	    /* The bold trick makes a single column of pixels appear in the
	     * next character.  When a bold character is removed, the next
	     * character should be redrawn too.  This happens for our own GUI
	     * and for some xterms. */
	    if (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# if defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    )
	    {
		hl = ScreenAttrs[off_to];
		if (hl > HL_ALL)
		    hl = syn_attr2attr(hl);
		if (hl & HL_BOLD)
		    redraw_next = TRUE;
	    }
#endif
	    ScreenAttrs[off_to] = ScreenAttrs[off_from];
#ifdef FEAT_MBYTE
	    /* For simplicity set the attributes of second half of a
	     * double-wide character equal to the first half. */
	    if (char_cells == 2)
		ScreenAttrs[off_to + 1] = ScreenAttrs[off_from];

	    if (enc_dbcs != 0 && char_cells == 2)
		screen_char_2(off_to, row, col + coloff);
	    else
#endif
		screen_char(off_to, row, col + coloff);
	}
	else if (  p_wiv
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		&& col + coloff > 0)
	{
	    if (ScreenAttrs[off_to] == ScreenAttrs[off_to - 1])
	    {
		/*
		 * Don't output stop-highlight when moving the cursor, it will
		 * stop the highlighting when it should continue.
		 */
		screen_attr = 0;
	    }
	    else if (screen_attr != 0)
		screen_stop_highlight();
	}

	off_to += CHAR_CELLS;
	off_from += CHAR_CELLS;
	col += CHAR_CELLS;
    }

#ifdef FEAT_MBYTE
    if (clear_next)
    {
	/* Clear the second half of a double-wide character of which the left
	 * half was overwritten with a single-wide character. */
	ScreenLines[off_to] = ' ';
	if (enc_utf8)
	    ScreenLinesUC[off_to] = 0;
	screen_char(off_to, row, col + coloff);
    }
#endif

    if (clear_width > 0
#ifdef FEAT_RIGHTLEFT
		    && !rlflag
#endif
				   )
    {
#ifdef FEAT_GUI
	int startCol = col;
#endif

	/* blank out the rest of the line */
	while (col < clear_width && ScreenLines[off_to] == ' '
						  && ScreenAttrs[off_to] == 0
#ifdef FEAT_MBYTE
				  && (!enc_utf8 || ScreenLinesUC[off_to] == 0)
#endif
						  )
	{
	    ++off_to;
	    ++col;
	}
	if (col < clear_width)
	{
#ifdef FEAT_GUI
	    /*
	     * In the GUI, clearing the rest of the line may leave pixels
	     * behind if the first character cleared was bold.  Some bold
	     * fonts spill over the left.  In this case we redraw the previous
	     * character too.  If we didn't skip any blanks above, then we
	     * only redraw if the character wasn't already redrawn anyway.
	     */
	    if (gui.in_use && (col > startCol || !redraw_this))
	    {
		hl = ScreenAttrs[off_to];
		if (hl > HL_ALL || (hl & HL_BOLD))
		{
		    int prev_cells = 1;
# ifdef FEAT_MBYTE
		    if (enc_utf8)
			/* for utf-8, ScreenLines[char_offset + 1] == 0 means
			 * that its width is 2. */
			prev_cells = ScreenLines[off_to - 1] == 0 ? 2 : 1;
		    else if (enc_dbcs != 0)
		    {
			/* find previous character by counting from first
			 * column and get its width. */
			unsigned off = LineOffset[row];
			unsigned max_off = LineOffset[row] + screen_Columns;

			while (off < off_to)
			{
			    prev_cells = (*mb_off2cells)(off, max_off);
			    off += prev_cells;
			}
		    }

		    if (enc_dbcs != 0 && prev_cells > 1)
			screen_char_2(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		    else
# endif
			screen_char(off_to - prev_cells, row,
						   col + coloff - prev_cells);
		}
	    }
#endif
	    screen_fill(row, row + 1, col + coloff, clear_width + coloff,
								 ' ', ' ', 0);
#ifdef FEAT_WINDOWS
	    off_to += clear_width - col;
	    col = clear_width;
#endif
	}
    }

    if (clear_width > 0)
    {
#ifdef FEAT_WINDOWS
	/* For a window that's left of another, draw the separator char. */
	if (col + coloff < Columns)
	{
	    int c;

	    c = fillchar_vsep(&hl);
	    if (ScreenLines[off_to] != (schar_T)c
# ifdef FEAT_MBYTE
		    || (enc_utf8 && (int)ScreenLinesUC[off_to]
						       != (c >= 0x80 ? c : 0))
# endif
		    || ScreenAttrs[off_to] != hl)
	    {
		ScreenLines[off_to] = c;
		ScreenAttrs[off_to] = hl;
# ifdef FEAT_MBYTE
		if (enc_utf8)
		{
		    if (c >= 0x80)
		    {
			ScreenLinesUC[off_to] = c;
			ScreenLinesC[0][off_to] = 0;
		    }
		    else
			ScreenLinesUC[off_to] = 0;
		}
# endif
		screen_char(off_to, row, col + coloff);
	    }
	}
	else
#endif
	    LineWraps[row] = FALSE;
    }
}

#if defined(FEAT_RIGHTLEFT) || defined(PROTO)
/*
 * Mirror text "str" for right-left displaying.
 * Only works for single-byte characters (e.g., numbers).
 */
    void
rl_mirror(char_u *str)
{
    char_u	*p1, *p2;
    int		t;

    for (p1 = str, p2 = str + STRLEN(str) - 1; p1 < p2; ++p1, --p2)
    {
	t = *p1;
	*p1 = *p2;
	*p2 = t;
    }
}
#endif

#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * mark all status lines for redraw; used after first :cd
 */
    void
status_redraw_all(void)
{
    win_T	*wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_status_height)
	{
	    wp->w_redr_status = TRUE;
	    redraw_later(VALID);
	}
}

/*
 * mark all status lines of the current buffer for redraw
 */
    void
status_redraw_curbuf(void)
{
    win_T	*wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_status_height != 0 && wp->w_buffer == curbuf)
	{
	    wp->w_redr_status = TRUE;
	    redraw_later(VALID);
	}
}

/*
 * Redraw all status lines that need to be redrawn.
 */
    void
redraw_statuslines(void)
{
    win_T	*wp;

    for (wp = firstwin; wp; wp = wp->w_next)
	if (wp->w_redr_status)
	    win_redr_status(wp);
    if (redraw_tabline)
	draw_tabline();
}
#endif

#if (defined(FEAT_WILDMENU) && defined(FEAT_WINDOWS)) || defined(PROTO)
/*
 * Redraw all status lines at the bottom of frame "frp".
 */
    void
win_redraw_last_status(frame_T *frp)
{
    if (frp->fr_layout == FR_LEAF)
	frp->fr_win->w_redr_status = TRUE;
    else if (frp->fr_layout == FR_ROW)
    {
	for (frp = frp->fr_child; frp != NULL; frp = frp->fr_next)
	    win_redraw_last_status(frp);
    }
    else /* frp->fr_layout == FR_COL */
    {
	frp = frp->fr_child;
	while (frp->fr_next != NULL)
	    frp = frp->fr_next;
	win_redraw_last_status(frp);
    }
}
#endif

#ifdef FEAT_WINDOWS
/*
 * Draw the verticap separator right of window "wp" starting with line "row".
 */
    static void
draw_vsep_win(win_T *wp, int row)
{
    int		hl;
    int		c;

    if (wp->w_vsep_width)
    {
	/* draw the vertical separator right of this window */
	c = fillchar_vsep(&hl);
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_ENDCOL(wp), W_ENDCOL(wp) + 1,
		c, ' ', hl);
    }
}
#endif

#ifdef FEAT_WILDMENU
static int status_match_len(expand_T *xp, char_u *s);
static int skip_status_match_char(expand_T *xp, char_u *s);

/*
 * Get the length of an item as it will be shown in the status line.
 */
    static int
status_match_len(expand_T *xp, char_u *s)
{
    int	len = 0;

#ifdef FEAT_MENU
    int emenu = (xp->xp_context == EXPAND_MENUS
	    || xp->xp_context == EXPAND_MENUNAMES);

    /* Check for menu separators - replace with '|'. */
    if (emenu && menu_is_separator(s))
	return 1;
#endif

    while (*s != NUL)
    {
	s += skip_status_match_char(xp, s);
	len += ptr2cells(s);
	mb_ptr_adv(s);
    }

    return len;
}

/*
 * Return the number of characters that should be skipped in a status match.
 * These are backslashes used for escaping.  Do show backslashes in help tags.
 */
    static int
skip_status_match_char(expand_T *xp, char_u *s)
{
    if ((rem_backslash(s) && xp->xp_context != EXPAND_HELP)
#ifdef FEAT_MENU
	    || ((xp->xp_context == EXPAND_MENUS
		    || xp->xp_context == EXPAND_MENUNAMES)
			  && (s[0] == '\t' || (s[0] == '\\' && s[1] != NUL)))
#endif
	   )
    {
#ifndef BACKSLASH_IN_FILENAME
	if (xp->xp_shell && csh_like_shell() && s[1] == '\\' && s[2] == '!')
	    return 2;
#endif
	return 1;
    }
    return 0;
}

/*
 * Show wildchar matches in the status line.
 * Show at least the "match" item.
 * We start at item 'first_match' in the list and show all matches that fit.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    void
win_redr_status_matches(
    expand_T	*xp,
    int		num_matches,
    char_u	**matches,	/* list of matches */
    int		match,
    int		showtail)
{
#define L_MATCH(m) (showtail ? sm_gettail(matches[m]) : matches[m])
    int		row;
    char_u	*buf;
    int		len;
    int		clen;		/* length in screen cells */
    int		fillchar;
    int		attr;
    int		i;
    int		highlight = TRUE;
    char_u	*selstart = NULL;
    int		selstart_col = 0;
    char_u	*selend = NULL;
    static int	first_match = 0;
    int		add_left = FALSE;
    char_u	*s;
#ifdef FEAT_MENU
    int		emenu;
#endif
#if defined(FEAT_MBYTE) || defined(FEAT_MENU)
    int		l;
#endif

    if (matches == NULL)	/* interrupted completion? */
	return;

#ifdef FEAT_MBYTE
    if (has_mbyte)
	buf = alloc((unsigned)Columns * MB_MAXBYTES + 1);
    else
#endif
	buf = alloc((unsigned)Columns + 1);
    if (buf == NULL)
	return;

    if (match == -1)	/* don't show match but original text */
    {
	match = 0;
	highlight = FALSE;
    }
    /* count 1 for the ending ">" */
    clen = status_match_len(xp, L_MATCH(match)) + 3;
    if (match == 0)
	first_match = 0;
    else if (match < first_match)
    {
	/* jumping left, as far as we can go */
	first_match = match;
	add_left = TRUE;
    }
    else
    {
	/* check if match fits on the screen */
	for (i = first_match; i < match; ++i)
	    clen += status_match_len(xp, L_MATCH(i)) + 2;
	if (first_match > 0)
	    clen += 2;
	/* jumping right, put match at the left */
	if ((long)clen > Columns)
	{
	    first_match = match;
	    /* if showing the last match, we can add some on the left */
	    clen = 2;
	    for (i = match; i < num_matches; ++i)
	    {
		clen += status_match_len(xp, L_MATCH(i)) + 2;
		if ((long)clen >= Columns)
		    break;
	    }
	    if (i == num_matches)
		add_left = TRUE;
	}
    }
    if (add_left)
	while (first_match > 0)
	{
	    clen += status_match_len(xp, L_MATCH(first_match - 1)) + 2;
	    if ((long)clen >= Columns)
		break;
	    --first_match;
	}

    fillchar = fillchar_status(&attr, TRUE);

    if (first_match == 0)
    {
	*buf = NUL;
	len = 0;
    }
    else
    {
	STRCPY(buf, "< ");
	len = 2;
    }
    clen = len;

    i = first_match;
    while ((long)(clen + status_match_len(xp, L_MATCH(i)) + 2) < Columns)
    {
	if (i == match)
	{
	    selstart = buf + len;
	    selstart_col = clen;
	}

	s = L_MATCH(i);
	/* Check for menu separators - replace with '|' */
#ifdef FEAT_MENU
	emenu = (xp->xp_context == EXPAND_MENUS
		|| xp->xp_context == EXPAND_MENUNAMES);
	if (emenu && menu_is_separator(s))
	{
	    STRCPY(buf + len, transchar('|'));
	    l = (int)STRLEN(buf + len);
	    len += l;
	    clen += l;
	}
	else
#endif
	    for ( ; *s != NUL; ++s)
	{
	    s += skip_status_match_char(xp, s);
	    clen += ptr2cells(s);
#ifdef FEAT_MBYTE
	    if (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	    {
		STRNCPY(buf + len, s, l);
		s += l - 1;
		len += l;
	    }
	    else
#endif
	    {
		STRCPY(buf + len, transchar_byte(*s));
		len += (int)STRLEN(buf + len);
	    }
	}
	if (i == match)
	    selend = buf + len;

	*(buf + len++) = ' ';
	*(buf + len++) = ' ';
	clen += 2;
	if (++i == num_matches)
		break;
    }

    if (i != num_matches)
    {
	*(buf + len++) = '>';
	++clen;
    }

    buf[len] = NUL;

    row = cmdline_row - 1;
    if (row >= 0)
    {
	if (wild_menu_showing == 0)
	{
	    if (msg_scrolled > 0)
	    {
		/* Put the wildmenu just above the command line.  If there is
		 * no room, scroll the screen one line up. */
		if (cmdline_row == Rows - 1)
		{
		    screen_del_lines(0, 0, 1, (int)Rows, TRUE, NULL);
		    ++msg_scrolled;
		}
		else
		{
		    ++cmdline_row;
		    ++row;
		}
		wild_menu_showing = WM_SCROLLED;
	    }
	    else
	    {
		/* Create status line if needed by setting 'laststatus' to 2.
		 * Set 'winminheight' to zero to avoid that the window is
		 * resized. */
		if (lastwin->w_status_height == 0)
		{
		    save_p_ls = p_ls;
		    save_p_wmh = p_wmh;
		    p_ls = 2;
		    p_wmh = 0;
		    last_status(FALSE);
		}
		wild_menu_showing = WM_SHOWN;
	    }
	}

	screen_puts(buf, row, 0, attr);
	if (selstart != NULL && highlight)
	{
	    *selend = NUL;
	    screen_puts(selstart, row, selstart_col, hl_attr(HLF_WM));
	}

	screen_fill(row, row + 1, clen, (int)Columns, fillchar, fillchar, attr);
    }

#ifdef FEAT_WINDOWS
    win_redraw_last_status(topframe);
#else
    lastwin->w_redr_status = TRUE;
#endif
    vim_free(buf);
}
#endif

#if defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * Redraw the status line of window wp.
 *
 * If inversion is possible we use it. Else '=' characters are used.
 */
    void
win_redr_status(win_T *wp)
{
    int		row;
    char_u	*p;
    int		len;
    int		fillchar;
    int		attr;
    int		this_ru_col;
    static int  busy = FALSE;

    /* It's possible to get here recursively when 'statusline' (indirectly)
     * invokes ":redrawstatus".  Simply ignore the call then. */
    if (busy)
	return;
    busy = TRUE;

    wp->w_redr_status = FALSE;
    if (wp->w_status_height == 0)
    {
	/* no status line, can only be last window */
	redraw_cmdline = TRUE;
    }
    else if (!redrawing()
#ifdef FEAT_INS_EXPAND
	    /* don't update status line when popup menu is visible and may be
	     * drawn over it */
	    || pum_visible()
#endif
	    )
    {
	/* Don't redraw right now, do it later. */
	wp->w_redr_status = TRUE;
    }
#ifdef FEAT_STL_OPT
    else if (*p_stl != NUL || *wp->w_p_stl != NUL)
    {
	/* redraw custom status line */
	redraw_custom_statusline(wp);
    }
#endif
    else
    {
	fillchar = fillchar_status(&attr, wp == curwin);

	get_trans_bufname(wp->w_buffer);
	p = NameBuff;
	len = (int)STRLEN(p);

	if (wp->w_buffer->b_help
#ifdef FEAT_QUICKFIX
		|| wp->w_p_pvw
#endif
		|| bufIsChanged(wp->w_buffer)
		|| wp->w_buffer->b_p_ro)
	    *(p + len++) = ' ';
	if (wp->w_buffer->b_help)
	{
	    STRCPY(p + len, _("[Help]"));
	    len += (int)STRLEN(p + len);
	}
#ifdef FEAT_QUICKFIX
	if (wp->w_p_pvw)
	{
	    STRCPY(p + len, _("[Preview]"));
	    len += (int)STRLEN(p + len);
	}
#endif
	if (bufIsChanged(wp->w_buffer))
	{
	    STRCPY(p + len, "[+]");
	    len += 3;
	}
	if (wp->w_buffer->b_p_ro)
	{
	    STRCPY(p + len, _("[RO]"));
	    len += 4;
	}

	this_ru_col = ru_col - (Columns - W_WIDTH(wp));
	if (this_ru_col < (W_WIDTH(wp) + 1) / 2)
	    this_ru_col = (W_WIDTH(wp) + 1) / 2;
	if (this_ru_col <= 1)
	{
	    p = (char_u *)"<";		/* No room for file name! */
	    len = 1;
	}
	else
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		int	clen = 0, i;

		/* Count total number of display cells. */
		clen = mb_string2cells(p, -1);

		/* Find first character that will fit.
		 * Going from start to end is much faster for DBCS. */
		for (i = 0; p[i] != NUL && clen >= this_ru_col - 1;
					      i += (*mb_ptr2len)(p + i))
		    clen -= (*mb_ptr2cells)(p + i);
		len = clen;
		if (i > 0)
		{
		    p = p + i - 1;
		    *p = '<';
		    ++len;
		}

	    }
	    else
#endif
	    if (len > this_ru_col - 1)
	    {
		p += len - (this_ru_col - 1);
		*p = '<';
		len = this_ru_col - 1;
	    }

	row = W_WINROW(wp) + wp->w_height;
	screen_puts(p, row, W_WINCOL(wp), attr);
	screen_fill(row, row + 1, len + W_WINCOL(wp),
			this_ru_col + W_WINCOL(wp), fillchar, fillchar, attr);

	if (get_keymap_str(wp, NameBuff, MAXPATHL)
		&& (int)(this_ru_col - len) > (int)(STRLEN(NameBuff) + 1))
	    screen_puts(NameBuff, row, (int)(this_ru_col - STRLEN(NameBuff)
						   - 1 + W_WINCOL(wp)), attr);

#ifdef FEAT_CMDL_INFO
	win_redr_ruler(wp, TRUE);
#endif
    }

    /*
     * May need to draw the character below the vertical separator.
     */
    if (wp->w_vsep_width != 0 && wp->w_status_height != 0 && redrawing())
    {
	if (stl_connected(wp))
	    fillchar = fillchar_status(&attr, wp == curwin);
	else
	    fillchar = fillchar_vsep(&attr);
	screen_putchar(fillchar, W_WINROW(wp) + wp->w_height, W_ENDCOL(wp),
									attr);
    }
    busy = FALSE;
}

#ifdef FEAT_STL_OPT
/*
 * Redraw the status line according to 'statusline' and take care of any
 * errors encountered.
 */
    static void
redraw_custom_statusline(win_T *wp)
{
    static int	    entered = FALSE;
    int		    save_called_emsg = called_emsg;

    /* When called recursively return.  This can happen when the statusline
     * contains an expression that triggers a redraw. */
    if (entered)
	return;
    entered = TRUE;

    called_emsg = FALSE;
    win_redr_custom(wp, FALSE);
    if (called_emsg)
    {
	/* When there is an error disable the statusline, otherwise the
	 * display is messed up with errors and a redraw triggers the problem
	 * again and again. */
	set_string_option_direct((char_u *)"statusline", -1,
		(char_u *)"", OPT_FREE | (*wp->w_p_stl != NUL
					? OPT_LOCAL : OPT_GLOBAL), SID_ERROR);
    }
    called_emsg |= save_called_emsg;
    entered = FALSE;
}
#endif

/*
 * Return TRUE if the status line of window "wp" is connected to the status
 * line of the window right of it.  If not, then it's a vertical separator.
 * Only call if (wp->w_vsep_width != 0).
 */
    int
stl_connected(win_T *wp)
{
    frame_T	*fr;

    fr = wp->w_frame;
    while (fr->fr_parent != NULL)
    {
	if (fr->fr_parent->fr_layout == FR_COL)
	{
	    if (fr->fr_next != NULL)
		break;
	}
	else
	{
	    if (fr->fr_next != NULL)
		return TRUE;
	}
	fr = fr->fr_parent;
    }
    return FALSE;
}

#endif /* FEAT_WINDOWS */

#if defined(FEAT_WINDOWS) || defined(FEAT_STL_OPT) || defined(PROTO)
/*
 * Get the value to show for the language mappings, active 'keymap'.
 */
    int
get_keymap_str(
    win_T	*wp,
    char_u	*buf,	    /* buffer for the result */
    int		len)	    /* length of buffer */
{
    char_u	*p;

    if (wp->w_buffer->b_p_iminsert != B_IMODE_LMAP)
	return FALSE;

    {
#ifdef FEAT_EVAL
	buf_T	*old_curbuf = curbuf;
	win_T	*old_curwin = curwin;
	char_u	*s;

	curbuf = wp->w_buffer;
	curwin = wp;
	STRCPY(buf, "b:keymap_name");	/* must be writable */
	++emsg_skip;
	s = p = eval_to_string(buf, NULL, FALSE);
	--emsg_skip;
	curbuf = old_curbuf;
	curwin = old_curwin;
	if (p == NULL || *p == NUL)
#endif
	{
#ifdef FEAT_KEYMAP
	    if (wp->w_buffer->b_kmap_state & KEYMAP_LOADED)
		p = wp->w_buffer->b_p_keymap;
	    else
#endif
		p = (char_u *)"lang";
	}
	if ((int)(STRLEN(p) + 3) < len)
	    sprintf((char *)buf, "<%s>", p);
	else
	    buf[0] = NUL;
#ifdef FEAT_EVAL
	vim_free(s);
#endif
    }
    return buf[0] != NUL;
}
#endif

#if defined(FEAT_STL_OPT) || defined(PROTO)
/*
 * Redraw the status line or ruler of window "wp".
 * When "wp" is NULL redraw the tab pages line from 'tabline'.
 */
    static void
win_redr_custom(
    win_T	*wp,
    int		draw_ruler)	/* TRUE or FALSE */
{
    static int	entered = FALSE;
    int		attr;
    int		curattr;
    int		row;
    int		col = 0;
    int		maxwidth;
    int		width;
    int		n;
    int		len;
    int		fillchar;
    char_u	buf[MAXPATHL];
    char_u	*stl;
    char_u	*p;
    struct	stl_hlrec hltab[STL_MAX_ITEM];
    struct	stl_hlrec tabtab[STL_MAX_ITEM];
    int		use_sandbox = FALSE;
    win_T	*ewp;
    int		p_crb_save;

    /* There is a tiny chance that this gets called recursively: When
     * redrawing a status line triggers redrawing the ruler or tabline.
     * Avoid trouble by not allowing recursion. */
    if (entered)
	return;
    entered = TRUE;

    /* setup environment for the task at hand */
    if (wp == NULL)
    {
	/* Use 'tabline'.  Always at the first line of the screen. */
	stl = p_tal;
	row = 0;
	fillchar = ' ';
	attr = hl_attr(HLF_TPF);
	maxwidth = Columns;
# ifdef FEAT_EVAL
	use_sandbox = was_set_insecurely((char_u *)"tabline", 0);
# endif
    }
    else
    {
	row = W_WINROW(wp) + wp->w_height;
	fillchar = fillchar_status(&attr, wp == curwin);
	maxwidth = W_WIDTH(wp);

	if (draw_ruler)
	{
	    stl = p_ruf;
	    /* advance past any leading group spec - implicit in ru_col */
	    if (*stl == '%')
	    {
		if (*++stl == '-')
		    stl++;
		if (atoi((char *)stl))
		    while (VIM_ISDIGIT(*stl))
			stl++;
		if (*stl++ != '(')
		    stl = p_ruf;
	    }
#ifdef FEAT_WINDOWS
	    col = ru_col - (Columns - W_WIDTH(wp));
	    if (col < (W_WIDTH(wp) + 1) / 2)
		col = (W_WIDTH(wp) + 1) / 2;
#else
	    col = ru_col;
	    if (col > (Columns + 1) / 2)
		col = (Columns + 1) / 2;
#endif
	    maxwidth = W_WIDTH(wp) - col;
#ifdef FEAT_WINDOWS
	    if (!wp->w_status_height)
#endif
	    {
		row = Rows - 1;
		--maxwidth;	/* writing in last column may cause scrolling */
		fillchar = ' ';
		attr = 0;
	    }

# ifdef FEAT_EVAL
	    use_sandbox = was_set_insecurely((char_u *)"rulerformat", 0);
# endif
	}
	else
	{
	    if (*wp->w_p_stl != NUL)
		stl = wp->w_p_stl;
	    else
		stl = p_stl;
# ifdef FEAT_EVAL
	    use_sandbox = was_set_insecurely((char_u *)"statusline",
					 *wp->w_p_stl == NUL ? 0 : OPT_LOCAL);
# endif
	}

#ifdef FEAT_WINDOWS
	col += W_WINCOL(wp);
#endif
    }

    if (maxwidth <= 0)
	goto theend;

    /* Temporarily reset 'cursorbind', we don't want a side effect from moving
     * the cursor away and back. */
    ewp = wp == NULL ? curwin : wp;
    p_crb_save = ewp->w_p_crb;
    ewp->w_p_crb = FALSE;

    /* Make a copy, because the statusline may include a function call that
     * might change the option value and free the memory. */
    stl = vim_strsave(stl);
    width = build_stl_str_hl(ewp, buf, sizeof(buf),
				stl, use_sandbox,
				fillchar, maxwidth, hltab, tabtab);
    vim_free(stl);
    ewp->w_p_crb = p_crb_save;

    /* Make all characters printable. */
    p = transstr(buf);
    if (p != NULL)
    {
	vim_strncpy(buf, p, sizeof(buf) - 1);
	vim_free(p);
    }

    /* fill up with "fillchar" */
    len = (int)STRLEN(buf);
    while (width < maxwidth && len < (int)sizeof(buf) - 1)
    {
#ifdef FEAT_MBYTE
	len += (*mb_char2bytes)(fillchar, buf + len);
#else
	buf[len++] = fillchar;
#endif
	++width;
    }
    buf[len] = NUL;

    /*
     * Draw each snippet with the specified highlighting.
     */
    curattr = attr;
    p = buf;
    for (n = 0; hltab[n].start != NULL; n++)
    {
	len = (int)(hltab[n].start - p);
	screen_puts_len(p, len, row, col, curattr);
	col += vim_strnsize(p, len);
	p = hltab[n].start;

	if (hltab[n].userhl == 0)
	    curattr = attr;
	else if (hltab[n].userhl < 0)
	    curattr = syn_id2attr(-hltab[n].userhl);
#ifdef FEAT_WINDOWS
	else if (wp != NULL && wp != curwin && wp->w_status_height != 0)
	    curattr = highlight_stlnc[hltab[n].userhl - 1];
#endif
	else
	    curattr = highlight_user[hltab[n].userhl - 1];
    }
    screen_puts(p, row, col, curattr);

    if (wp == NULL)
    {
	/* Fill the TabPageIdxs[] array for clicking in the tab pagesline. */
	col = 0;
	len = 0;
	p = buf;
	fillchar = 0;
	for (n = 0; tabtab[n].start != NULL; n++)
	{
	    len += vim_strnsize(p, (int)(tabtab[n].start - p));
	    while (col < len)
		TabPageIdxs[col++] = fillchar;
	    p = tabtab[n].start;
	    fillchar = tabtab[n].userhl;
	}
	while (col < Columns)
	    TabPageIdxs[col++] = fillchar;
    }

theend:
    entered = FALSE;
}

#endif /* FEAT_STL_OPT */

/*
 * Output a single character directly to the screen and update ScreenLines.
 */
    void
screen_putchar(int c, int row, int col, int attr)
{
    char_u	buf[MB_MAXBYTES + 1];

#ifdef FEAT_MBYTE
    if (has_mbyte)
	buf[(*mb_char2bytes)(c, buf)] = NUL;
    else
#endif
    {
	buf[0] = c;
	buf[1] = NUL;
    }
    screen_puts(buf, row, col, attr);
}

/*
 * Get a single character directly from ScreenLines into "bytes[]".
 * Also return its attribute in *attrp;
 */
    void
screen_getbytes(int row, int col, char_u *bytes, int *attrp)
{
    unsigned off;

    /* safety check */
    if (ScreenLines != NULL && row < screen_Rows && col < screen_Columns)
    {
	off = LineOffset[row] + col;
	*attrp = ScreenAttrs[off];
	bytes[0] = ScreenLines[off];
	bytes[1] = NUL;

#ifdef FEAT_MBYTE
	if (enc_utf8 && ScreenLinesUC[off] != 0)
	    bytes[utfc_char2bytes(off, bytes)] = NUL;
	else if (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
	{
	    bytes[0] = ScreenLines[off];
	    bytes[1] = ScreenLines2[off];
	    bytes[2] = NUL;
	}
	else if (enc_dbcs && MB_BYTE2LEN(bytes[0]) > 1)
	{
	    bytes[1] = ScreenLines[off + 1];
	    bytes[2] = NUL;
	}
#endif
    }
}

#ifdef FEAT_MBYTE
static int screen_comp_differs(int, int*);

/*
 * Return TRUE if composing characters for screen posn "off" differs from
 * composing characters in "u8cc".
 * Only to be used when ScreenLinesUC[off] != 0.
 */
    static int
screen_comp_differs(int off, int *u8cc)
{
    int	    i;

    for (i = 0; i < Screen_mco; ++i)
    {
	if (ScreenLinesC[i][off] != (u8char_T)u8cc[i])
	    return TRUE;
	if (u8cc[i] == 0)
	    break;
    }
    return FALSE;
}
#endif

/*
 * Put string '*text' on the screen at position 'row' and 'col', with
 * attributes 'attr', and update ScreenLines[] and ScreenAttrs[].
 * Note: only outputs within one row, message is truncated at screen boundary!
 * Note: if ScreenLines[], row and/or col is invalid, nothing is done.
 */
    void
screen_puts(
    char_u	*text,
    int		row,
    int		col,
    int		attr)
{
    screen_puts_len(text, -1, row, col, attr);
}

/*
 * Like screen_puts(), but output "text[len]".  When "len" is -1 output up to
 * a NUL.
 */
    void
screen_puts_len(
    char_u	*text,
    int		textlen,
    int		row,
    int		col,
    int		attr)
{
    unsigned	off;
    char_u	*ptr = text;
    int		len = textlen;
    int		c;
#ifdef FEAT_MBYTE
    unsigned	max_off;
    int		mbyte_blen = 1;
    int		mbyte_cells = 1;
    int		u8c = 0;
    int		u8cc[MAX_MCO];
    int		clear_next_cell = FALSE;
# ifdef FEAT_ARABIC
    int		prev_c = 0;		/* previous Arabic character */
    int		pc, nc, nc1;
    int		pcc[MAX_MCO];
# endif
#endif
#if defined(FEAT_MBYTE) || defined(FEAT_GUI) || defined(UNIX)
    int		force_redraw_this;
    int		force_redraw_next = FALSE;
#endif
    int		need_redraw;

    if (ScreenLines == NULL || row >= screen_Rows)	/* safety check */
	return;
    off = LineOffset[row] + col;

#ifdef FEAT_MBYTE
    /* When drawing over the right halve of a double-wide char clear out the
     * left halve.  Only needed in a terminal. */
    if (has_mbyte && col > 0 && col < screen_Columns
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    && mb_fix_col(col, row) != col)
    {
	ScreenLines[off - 1] = ' ';
	ScreenAttrs[off - 1] = 0;
	if (enc_utf8)
	{
	    ScreenLinesUC[off - 1] = 0;
	    ScreenLinesC[0][off - 1] = 0;
	}
	/* redraw the previous cell, make it empty */
	screen_char(off - 1, row, col - 1);
	/* force the cell at "col" to be redrawn */
	force_redraw_next = TRUE;
    }
#endif

#ifdef FEAT_MBYTE
    max_off = LineOffset[row] + screen_Columns;
#endif
    while (col < screen_Columns
	    && (len < 0 || (int)(ptr - text) < len)
	    && *ptr != NUL)
    {
	c = *ptr;
#ifdef FEAT_MBYTE
	/* check if this is the first byte of a multibyte */
	if (has_mbyte)
	{
	    if (enc_utf8 && len > 0)
		mbyte_blen = utfc_ptr2len_len(ptr, (int)((text + len) - ptr));
	    else
		mbyte_blen = (*mb_ptr2len)(ptr);
	    if (enc_dbcs == DBCS_JPNU && c == 0x8e)
		mbyte_cells = 1;
	    else if (enc_dbcs != 0)
		mbyte_cells = mbyte_blen;
	    else	/* enc_utf8 */
	    {
		if (len >= 0)
		    u8c = utfc_ptr2char_len(ptr, u8cc,
						   (int)((text + len) - ptr));
		else
		    u8c = utfc_ptr2char(ptr, u8cc);
		mbyte_cells = utf_char2cells(u8c);
# ifdef UNICODE16
		/* Non-BMP character: display as ? or fullwidth ?. */
		if (u8c >= 0x10000)
		{
		    u8c = (mbyte_cells == 2) ? 0xff1f : (int)'?';
		    if (attr == 0)
			attr = hl_attr(HLF_8);
		}
# endif
# ifdef FEAT_ARABIC
		if (p_arshape && !p_tbidi && ARABIC_CHAR(u8c))
		{
		    /* Do Arabic shaping. */
		    if (len >= 0 && (int)(ptr - text) + mbyte_blen >= len)
		    {
			/* Past end of string to be displayed. */
			nc = NUL;
			nc1 = NUL;
		    }
		    else
		    {
			nc = utfc_ptr2char_len(ptr + mbyte_blen, pcc,
				      (int)((text + len) - ptr - mbyte_blen));
			nc1 = pcc[0];
		    }
		    pc = prev_c;
		    prev_c = u8c;
		    u8c = arabic_shape(u8c, &c, &u8cc[0], nc, nc1, pc);
		}
		else
		    prev_c = u8c;
# endif
		if (col + mbyte_cells > screen_Columns)
		{
		    /* Only 1 cell left, but character requires 2 cells:
		     * display a '>' in the last column to avoid wrapping. */
		    c = '>';
		    mbyte_cells = 1;
		}
	    }
	}
#endif

#if defined(FEAT_MBYTE) || defined(FEAT_GUI) || defined(UNIX)
	force_redraw_this = force_redraw_next;
	force_redraw_next = FALSE;
#endif

	need_redraw = ScreenLines[off] != c
#ifdef FEAT_MBYTE
		|| (mbyte_cells == 2
		    && ScreenLines[off + 1] != (enc_dbcs ? ptr[1] : 0))
		|| (enc_dbcs == DBCS_JPNU
		    && c == 0x8e
		    && ScreenLines2[off] != ptr[1])
		|| (enc_utf8
		    && (ScreenLinesUC[off] !=
				(u8char_T)(c < 0x80 && u8cc[0] == 0 ? 0 : u8c)
			|| (ScreenLinesUC[off] != 0
					  && screen_comp_differs(off, u8cc))))
#endif
		|| ScreenAttrs[off] != attr
		|| exmode_active;

	if (need_redraw
#if defined(FEAT_MBYTE) || defined(FEAT_GUI) || defined(UNIX)
		|| force_redraw_this
#endif
		)
	{
#if defined(FEAT_GUI) || defined(UNIX)
	    /* The bold trick makes a single row of pixels appear in the next
	     * character.  When a bold character is removed, the next
	     * character should be redrawn too.  This happens for our own GUI
	     * and for some xterms. */
	    if (need_redraw && ScreenLines[off] != ' ' && (
# ifdef FEAT_GUI
		    gui.in_use
# endif
# if defined(FEAT_GUI) && defined(UNIX)
		    ||
# endif
# ifdef UNIX
		    term_is_xterm
# endif
		    ))
	    {
		int	n = ScreenAttrs[off];

		if (n > HL_ALL)
		    n = syn_attr2attr(n);
		if (n & HL_BOLD)
		    force_redraw_next = TRUE;
	    }
#endif
#ifdef FEAT_MBYTE
	    /* When at the end of the text and overwriting a two-cell
	     * character with a one-cell character, need to clear the next
	     * cell.  Also when overwriting the left halve of a two-cell char
	     * with the right halve of a two-cell char.  Do this only once
	     * (mb_off2cells() may return 2 on the right halve). */
	    if (clear_next_cell)
		clear_next_cell = FALSE;
	    else if (has_mbyte
		    && (len < 0 ? ptr[mbyte_blen] == NUL
					     : ptr + mbyte_blen >= text + len)
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		clear_next_cell = TRUE;

	    /* Make sure we never leave a second byte of a double-byte behind,
	     * it confuses mb_off2cells(). */
	    if (enc_dbcs
		    && ((mbyte_cells == 1 && (*mb_off2cells)(off, max_off) > 1)
			|| (mbyte_cells == 2
			    && (*mb_off2cells)(off, max_off) == 1
			    && (*mb_off2cells)(off + 1, max_off) > 1)))
		ScreenLines[off + mbyte_blen] = 0;
#endif
	    ScreenLines[off] = c;
	    ScreenAttrs[off] = attr;
#ifdef FEAT_MBYTE
	    if (enc_utf8)
	    {
		if (c < 0x80 && u8cc[0] == 0)
		    ScreenLinesUC[off] = 0;
		else
		{
		    int	    i;

		    ScreenLinesUC[off] = u8c;
		    for (i = 0; i < Screen_mco; ++i)
		    {
			ScreenLinesC[i][off] = u8cc[i];
			if (u8cc[i] == 0)
			    break;
		    }
		}
		if (mbyte_cells == 2)
		{
		    ScreenLines[off + 1] = 0;
		    ScreenAttrs[off + 1] = attr;
		}
		screen_char(off, row, col);
	    }
	    else if (mbyte_cells == 2)
	    {
		ScreenLines[off + 1] = ptr[1];
		ScreenAttrs[off + 1] = attr;
		screen_char_2(off, row, col);
	    }
	    else if (enc_dbcs == DBCS_JPNU && c == 0x8e)
	    {
		ScreenLines2[off] = ptr[1];
		screen_char(off, row, col);
	    }
	    else
#endif
		screen_char(off, row, col);
	}
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    off += mbyte_cells;
	    col += mbyte_cells;
	    ptr += mbyte_blen;
	    if (clear_next_cell)
	    {
		/* This only happens at the end, display one space next. */
		ptr = (char_u *)" ";
		len = -1;
	    }
	}
	else
#endif
	{
	    ++off;
	    ++col;
	    ++ptr;
	}
    }

#if defined(FEAT_MBYTE) || defined(FEAT_GUI) || defined(UNIX)
    /* If we detected the next character needs to be redrawn, but the text
     * doesn't extend up to there, update the character here. */
    if (force_redraw_next && col < screen_Columns)
    {
# ifdef FEAT_MBYTE
	if (enc_dbcs != 0 && dbcs_off2cells(off, max_off) > 1)
	    screen_char_2(off, row, col);
	else
# endif
	    screen_char(off, row, col);
    }
#endif
}

#ifdef FEAT_SEARCH_EXTRA
/*
 * Prepare for 'hlsearch' highlighting.
 */
    static void
start_search_hl(void)
{
    if (p_hls && !no_hlsearch)
    {
	last_pat_prog(&search_hl.rm);
	search_hl.attr = hl_attr(HLF_L);
# ifdef FEAT_RELTIME
	/* Set the time limit to 'redrawtime'. */
	profile_setlimit(p_rdt, &search_hl.tm);
# endif
    }
}

/*
 * Clean up for 'hlsearch' highlighting.
 */
    static void
end_search_hl(void)
{
    if (search_hl.rm.regprog != NULL)
    {
	vim_regfree(search_hl.rm.regprog);
	search_hl.rm.regprog = NULL;
    }
}

/*
 * Init for calling prepare_search_hl().
 */
    static void
init_search_hl(win_T *wp)
{
    matchitem_T *cur;

    /* Setup for match and 'hlsearch' highlighting.  Disable any previous
     * match */
    cur = wp->w_match_head;
    while (cur != NULL)
    {
	cur->hl.rm = cur->match;
	if (cur->hlg_id == 0)
	    cur->hl.attr = 0;
	else
	    cur->hl.attr = syn_id2attr(cur->hlg_id);
	cur->hl.buf = wp->w_buffer;
	cur->hl.lnum = 0;
	cur->hl.first_lnum = 0;
# ifdef FEAT_RELTIME
	/* Set the time limit to 'redrawtime'. */
	profile_setlimit(p_rdt, &(cur->hl.tm));
# endif
	cur = cur->next;
    }
    search_hl.buf = wp->w_buffer;
    search_hl.lnum = 0;
    search_hl.first_lnum = 0;
    /* time limit is set at the toplevel, for all windows */
}

/*
 * Advance to the match in window "wp" line "lnum" or past it.
 */
    static void
prepare_search_hl(win_T *wp, linenr_T lnum)
{
    matchitem_T *cur;		/* points to the match list */
    match_T	*shl;		/* points to search_hl or a match */
    int		shl_flag;	/* flag to indicate whether search_hl
				   has been processed or not */
    int		pos_inprogress;	/* marks that position match search is
				   in progress */
    int		n;

    /*
     * When using a multi-line pattern, start searching at the top
     * of the window or just after a closed fold.
     * Do this both for search_hl and the match list.
     */
    cur = wp->w_match_head;
    shl_flag = FALSE;
    while (cur != NULL || shl_flag == FALSE)
    {
	if (shl_flag == FALSE)
	{
	    shl = &search_hl;
	    shl_flag = TRUE;
	}
	else
	    shl = &cur->hl;
	if (shl->rm.regprog != NULL
		&& shl->lnum == 0
		&& re_multiline(shl->rm.regprog))
	{
	    if (shl->first_lnum == 0)
	    {
# ifdef FEAT_FOLDING
		for (shl->first_lnum = lnum;
			   shl->first_lnum > wp->w_topline; --shl->first_lnum)
		    if (hasFoldingWin(wp, shl->first_lnum - 1,
						      NULL, NULL, TRUE, NULL))
			break;
# else
		shl->first_lnum = wp->w_topline;
# endif
	    }
	    if (cur != NULL)
		cur->pos.cur = 0;
	    pos_inprogress = TRUE;
	    n = 0;
	    while (shl->first_lnum < lnum && (shl->rm.regprog != NULL
					  || (cur != NULL && pos_inprogress)))
	    {
		next_search_hl(wp, shl, shl->first_lnum, (colnr_T)n, cur);
		pos_inprogress = cur == NULL || cur->pos.cur == 0
							      ? FALSE : TRUE;
		if (shl->lnum != 0)
		{
		    shl->first_lnum = shl->lnum
				    + shl->rm.endpos[0].lnum
				    - shl->rm.startpos[0].lnum;
		    n = shl->rm.endpos[0].col;
		}
		else
		{
		    ++shl->first_lnum;
		    n = 0;
		}
	    }
	}
	if (shl != &search_hl && cur != NULL)
	    cur = cur->next;
    }
}

/*
 * Search for a next 'hlsearch' or match.
 * Uses shl->buf.
 * Sets shl->lnum and shl->rm contents.
 * Note: Assumes a previous match is always before "lnum", unless
 * shl->lnum is zero.
 * Careful: Any pointers for buffer lines will become invalid.
 */
    static void
next_search_hl(
    win_T	    *win,
    match_T	    *shl,	/* points to search_hl or a match */
    linenr_T	    lnum,
    colnr_T	    mincol,	/* minimal column for a match */
    matchitem_T	    *cur)	/* to retrieve match positions if any */
{
    linenr_T	l;
    colnr_T	matchcol;
    long	nmatched;

    if (shl->lnum != 0)
    {
	/* Check for three situations:
	 * 1. If the "lnum" is below a previous match, start a new search.
	 * 2. If the previous match includes "mincol", use it.
	 * 3. Continue after the previous match.
	 */
	l = shl->lnum + shl->rm.endpos[0].lnum - shl->rm.startpos[0].lnum;
	if (lnum > l)
	    shl->lnum = 0;
	else if (lnum < l || shl->rm.endpos[0].col > mincol)
	    return;
    }

    /*
     * Repeat searching for a match until one is found that includes "mincol"
     * or none is found in this line.
     */
    called_emsg = FALSE;
    for (;;)
    {
#ifdef FEAT_RELTIME
	/* Stop searching after passing the time limit. */
	if (profile_passed_limit(&(shl->tm)))
	{
	    shl->lnum = 0;		/* no match found in time */
	    break;
	}
#endif
	/* Three situations:
	 * 1. No useful previous match: search from start of line.
	 * 2. Not Vi compatible or empty match: continue at next character.
	 *    Break the loop if this is beyond the end of the line.
	 * 3. Vi compatible searching: continue at end of previous match.
	 */
	if (shl->lnum == 0)
	    matchcol = 0;
	else if (vim_strchr(p_cpo, CPO_SEARCH) == NULL
		|| (shl->rm.endpos[0].lnum == 0
		    && shl->rm.endpos[0].col <= shl->rm.startpos[0].col))
	{
	    char_u	*ml;

	    matchcol = shl->rm.startpos[0].col;
	    ml = ml_get_buf(shl->buf, lnum, FALSE) + matchcol;
	    if (*ml == NUL)
	    {
		++matchcol;
		shl->lnum = 0;
		break;
	    }
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		matchcol += mb_ptr2len(ml);
	    else
#endif
		++matchcol;
	}
	else
	    matchcol = shl->rm.endpos[0].col;

	shl->lnum = lnum;
	if (shl->rm.regprog != NULL)
	{
	    /* Remember whether shl->rm is using a copy of the regprog in
	     * cur->match. */
	    int regprog_is_copy = (shl != &search_hl && cur != NULL
				&& shl == &cur->hl
				&& cur->match.regprog == cur->hl.rm.regprog);

	    nmatched = vim_regexec_multi(&shl->rm, win, shl->buf, lnum,
		    matchcol,
#ifdef FEAT_RELTIME
		    &(shl->tm)
#else
		    NULL
#endif
		    );
	    /* Copy the regprog, in case it got freed and recompiled. */
	    if (regprog_is_copy)
		cur->match.regprog = cur->hl.rm.regprog;

	    if (called_emsg || got_int)
	    {
		/* Error while handling regexp: stop using this regexp. */
		if (shl == &search_hl)
		{
		    /* don't free regprog in the match list, it's a copy */
		    vim_regfree(shl->rm.regprog);
		    SET_NO_HLSEARCH(TRUE);
		}
		shl->rm.regprog = NULL;
		shl->lnum = 0;
		got_int = FALSE;  /* avoid the "Type :quit to exit Vim"
				     message */
		break;
	    }
	}
	else if (cur != NULL)
	    nmatched = next_search_hl_pos(shl, lnum, &(cur->pos), matchcol);
	else
	    nmatched = 0;
	if (nmatched == 0)
	{
	    shl->lnum = 0;		/* no match found */
	    break;
	}
	if (shl->rm.startpos[0].lnum > 0
		|| shl->rm.startpos[0].col >= mincol
		|| nmatched > 1
		|| shl->rm.endpos[0].col > mincol)
	{
	    shl->lnum += shl->rm.startpos[0].lnum;
	    break;			/* useful match found */
	}
    }
}

    static int
next_search_hl_pos(
    match_T	    *shl,	/* points to a match */
    linenr_T	    lnum,
    posmatch_T	    *posmatch,	/* match positions */
    colnr_T	    mincol)	/* minimal column for a match */
{
    int	    i;
    int	    bot = -1;

    shl->lnum = 0;
    for (i = posmatch->cur; i < MAXPOSMATCH; i++)
    {
	if (posmatch->pos[i].lnum == 0)
	    break;
	if (posmatch->pos[i].col < mincol)
	    continue;
	if (posmatch->pos[i].lnum == lnum)
	{
	    if (shl->lnum == lnum)
	    {
		/* partially sort positions by column numbers
		 * on the same line */
		if (posmatch->pos[i].col < posmatch->pos[bot].col)
		{
		    llpos_T	tmp = posmatch->pos[i];

		    posmatch->pos[i] = posmatch->pos[bot];
		    posmatch->pos[bot] = tmp;
		}
	    }
	    else
	    {
		bot = i;
		shl->lnum = lnum;
	    }
	}
    }
    posmatch->cur = 0;
    if (shl->lnum == lnum && bot >= 0)
    {
	colnr_T	start = posmatch->pos[bot].col == 0
					     ? 0 : posmatch->pos[bot].col - 1;
	colnr_T	end = posmatch->pos[bot].col == 0
				    ? MAXCOL : start + posmatch->pos[bot].len;

	shl->rm.startpos[0].lnum = 0;
	shl->rm.startpos[0].col = start;
	shl->rm.endpos[0].lnum = 0;
	shl->rm.endpos[0].col = end;
	posmatch->cur = bot + 1;
	return TRUE;
    }
    return FALSE;
}
#endif

      static void
screen_start_highlight(int attr)
{
    attrentry_T *aep = NULL;

    screen_attr = attr;
    if (full_screen
#ifdef WIN3264
		    && termcap_active
#endif
				       )
    {
#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    char	buf[20];

	    /* The GUI handles this internally. */
	    sprintf(buf, IF_EB("\033|%dh", ESC_STR "|%dh"), attr);
	    OUT_STR(buf);
	}
	else
#endif
	{
	    if (attr > HL_ALL)				/* special HL attr. */
	    {
		if (t_colors > 1)
		    aep = syn_cterm_attr2entry(attr);
		else
		    aep = syn_term_attr2entry(attr);
		if (aep == NULL)	    /* did ":syntax clear" */
		    attr = 0;
		else
		    attr = aep->ae_attr;
	    }
	    if ((attr & HL_BOLD) && T_MD != NULL)	/* bold */
		out_str(T_MD);
	    else if (aep != NULL && t_colors > 1 && aep->ae_u.cterm.fg_color
						      && cterm_normal_fg_bold)
		/* If the Normal FG color has BOLD attribute and the new HL
		 * has a FG color defined, clear BOLD. */
		out_str(T_ME);
	    if ((attr & HL_STANDOUT) && T_SO != NULL)	/* standout */
		out_str(T_SO);
	    if ((attr & (HL_UNDERLINE | HL_UNDERCURL)) && T_US != NULL)
						   /* underline or undercurl */
		out_str(T_US);
	    if ((attr & HL_ITALIC) && T_CZH != NULL)	/* italic */
		out_str(T_CZH);
	    if ((attr & HL_INVERSE) && T_MR != NULL)	/* inverse (reverse) */
		out_str(T_MR);

	    /*
	     * Output the color or start string after bold etc., in case the
	     * bold etc. override the color setting.
	     */
	    if (aep != NULL)
	    {
		if (t_colors > 1)
		{
		    if (aep->ae_u.cterm.fg_color)
			term_fg_color(aep->ae_u.cterm.fg_color - 1);
		    if (aep->ae_u.cterm.bg_color)
			term_bg_color(aep->ae_u.cterm.bg_color - 1);
		}
		else
		{
		    if (aep->ae_u.term.start != NULL)
			out_str(aep->ae_u.term.start);
		}
	    }
	}
    }
}

      void
screen_stop_highlight(void)
{
    int	    do_ME = FALSE;	    /* output T_ME code */

    if (screen_attr != 0
#ifdef WIN3264
			&& termcap_active
#endif
					   )
    {
#ifdef FEAT_GUI
	if (gui.in_use)
	{
	    char	buf[20];

	    /* use internal GUI code */
	    sprintf(buf, IF_EB("\033|%dH", ESC_STR "|%dH"), screen_attr);
	    OUT_STR(buf);
	}
	else
#endif
	{
	    if (screen_attr > HL_ALL)			/* special HL attr. */
	    {
		attrentry_T *aep;

		if (t_colors > 1)
		{
		    /*
		     * Assume that t_me restores the original colors!
		     */
		    aep = syn_cterm_attr2entry(screen_attr);
		    if (aep != NULL && (aep->ae_u.cterm.fg_color
						 || aep->ae_u.cterm.bg_color))
			do_ME = TRUE;
		}
		else
		{
		    aep = syn_term_attr2entry(screen_attr);
		    if (aep != NULL && aep->ae_u.term.stop != NULL)
		    {
			if (STRCMP(aep->ae_u.term.stop, T_ME) == 0)
			    do_ME = TRUE;
			else
			    out_str(aep->ae_u.term.stop);
		    }
		}
		if (aep == NULL)	    /* did ":syntax clear" */
		    screen_attr = 0;
		else
		    screen_attr = aep->ae_attr;
	    }

	    /*
	     * Often all ending-codes are equal to T_ME.  Avoid outputting the
	     * same sequence several times.
	     */
	    if (screen_attr & HL_STANDOUT)
	    {
		if (STRCMP(T_SE, T_ME) == 0)
		    do_ME = TRUE;
		else
		    out_str(T_SE);
	    }
	    if (screen_attr & (HL_UNDERLINE | HL_UNDERCURL))
	    {
		if (STRCMP(T_UE, T_ME) == 0)
		    do_ME = TRUE;
		else
		    out_str(T_UE);
	    }
	    if (screen_attr & HL_ITALIC)
	    {
		if (STRCMP(T_CZR, T_ME) == 0)
		    do_ME = TRUE;
		else
		    out_str(T_CZR);
	    }
	    if (do_ME || (screen_attr & (HL_BOLD | HL_INVERSE)))
		out_str(T_ME);

	    if (t_colors > 1)
	    {
		/* set Normal cterm colors */
		if (cterm_normal_fg_color != 0)
		    term_fg_color(cterm_normal_fg_color - 1);
		if (cterm_normal_bg_color != 0)
		    term_bg_color(cterm_normal_bg_color - 1);
		if (cterm_normal_fg_bold)
		    out_str(T_MD);
	    }
	}
    }
    screen_attr = 0;
}

/*
 * Reset the colors for a cterm.  Used when leaving Vim.
 * The machine specific code may override this again.
 */
    void
reset_cterm_colors(void)
{
    if (t_colors > 1)
    {
	/* set Normal cterm colors */
	if (cterm_normal_fg_color > 0 || cterm_normal_bg_color > 0)
	{
	    out_str(T_OP);
	    screen_attr = -1;
	}
	if (cterm_normal_fg_bold)
	{
	    out_str(T_ME);
	    screen_attr = -1;
	}
    }
}

/*
 * Put character ScreenLines["off"] on the screen at position "row" and "col",
 * using the attributes from ScreenAttrs["off"].
 */
    static void
screen_char(unsigned off, int row, int col)
{
    int		attr;

    /* Check for illegal values, just in case (could happen just after
     * resizing). */
    if (row >= screen_Rows || col >= screen_Columns)
	return;

    /* Outputting a character in the last cell on the screen may scroll the
     * screen up.  Only do it when the "xn" termcap property is set, otherwise
     * mark the character invalid (update it when scrolled up). */
    if (*T_XN == NUL
	    && row == screen_Rows - 1 && col == screen_Columns - 1
#ifdef FEAT_RIGHTLEFT
	    /* account for first command-line character in rightleft mode */
	    && !cmdmsg_rl
#endif
       )
    {
	ScreenAttrs[off] = (sattr_T)-1;
	return;
    }

    /*
     * Stop highlighting first, so it's easier to move the cursor.
     */
#if defined(FEAT_CLIPBOARD) || defined(FEAT_WINDOWS)
    if (screen_char_attr != 0)
	attr = screen_char_attr;
    else
#endif
	attr = ScreenAttrs[off];
    if (screen_attr != attr)
	screen_stop_highlight();

    windgoto(row, col);

    if (screen_attr != attr)
	screen_start_highlight(attr);

#ifdef FEAT_MBYTE
    if (enc_utf8 && ScreenLinesUC[off] != 0)
    {
	char_u	    buf[MB_MAXBYTES + 1];

	/* Convert UTF-8 character to bytes and write it. */

	buf[utfc_char2bytes(off, buf)] = NUL;

	out_str(buf);
	if (utf_char2cells(ScreenLinesUC[off]) > 1)
	    ++screen_cur_col;
    }
    else
#endif
    {
#ifdef FEAT_MBYTE
	out_flush_check();
#endif
	out_char(ScreenLines[off]);
#ifdef FEAT_MBYTE
	/* double-byte character in single-width cell */
	if (enc_dbcs == DBCS_JPNU && ScreenLines[off] == 0x8e)
	    out_char(ScreenLines2[off]);
#endif
    }

    screen_cur_col++;
}

#ifdef FEAT_MBYTE

/*
 * Used for enc_dbcs only: Put one double-wide character at ScreenLines["off"]
 * on the screen at position 'row' and 'col'.
 * The attributes of the first byte is used for all.  This is required to
 * output the two bytes of a double-byte character with nothing in between.
 */
    static void
screen_char_2(unsigned off, int row, int col)
{
    /* Check for illegal values (could be wrong when screen was resized). */
    if (off + 1 >= (unsigned)(screen_Rows * screen_Columns))
	return;

    /* Outputting the last character on the screen may scrollup the screen.
     * Don't to it!  Mark the character invalid (update it when scrolled up) */
    if (row == screen_Rows - 1 && col >= screen_Columns - 2)
    {
	ScreenAttrs[off] = (sattr_T)-1;
	return;
    }

    /* Output the first byte normally (positions the cursor), then write the
     * second byte directly. */
    screen_char(off, row, col);
    out_char(ScreenLines[off + 1]);
    ++screen_cur_col;
}
#endif

#if defined(FEAT_CLIPBOARD) || defined(FEAT_WINDOWS) || defined(PROTO)
/*
 * Draw a rectangle of the screen, inverted when "invert" is TRUE.
 * This uses the contents of ScreenLines[] and doesn't change it.
 */
    void
screen_draw_rectangle(
    int		row,
    int		col,
    int		height,
    int		width,
    int		invert)
{
    int		r, c;
    int		off;
#ifdef FEAT_MBYTE
    int		max_off;
#endif

    /* Can't use ScreenLines unless initialized */
    if (ScreenLines == NULL)
	return;

    if (invert)
	screen_char_attr = HL_INVERSE;
    for (r = row; r < row + height; ++r)
    {
	off = LineOffset[r];
#ifdef FEAT_MBYTE
	max_off = off + screen_Columns;
#endif
	for (c = col; c < col + width; ++c)
	{
#ifdef FEAT_MBYTE
	    if (enc_dbcs != 0 && dbcs_off2cells(off + c, max_off) > 1)
	    {
		screen_char_2(off + c, r, c);
		++c;
	    }
	    else
#endif
	    {
		screen_char(off + c, r, c);
#ifdef FEAT_MBYTE
		if (utf_off2cells(off + c, max_off) > 1)
		    ++c;
#endif
	    }
	}
    }
    screen_char_attr = 0;
}
#endif

#ifdef FEAT_WINDOWS
/*
 * Redraw the characters for a vertically split window.
 */
    static void
redraw_block(int row, int end, win_T *wp)
{
    int		col;
    int		width;

# ifdef FEAT_CLIPBOARD
    clip_may_clear_selection(row, end - 1);
# endif

    if (wp == NULL)
    {
	col = 0;
	width = Columns;
    }
    else
    {
	col = wp->w_wincol;
	width = wp->w_width;
    }
    screen_draw_rectangle(row, col, end - row, width, FALSE);
}
#endif

/*
 * Fill the screen from 'start_row' to 'end_row', from 'start_col' to 'end_col'
 * with character 'c1' in first column followed by 'c2' in the other columns.
 * Use attributes 'attr'.
 */
    void
screen_fill(
    int	    start_row,
    int	    end_row,
    int	    start_col,
    int	    end_col,
    int	    c1,
    int	    c2,
    int	    attr)
{
    int		    row;
    int		    col;
    int		    off;
    int		    end_off;
    int		    did_delete;
    int		    c;
    int		    norm_term;
#if defined(FEAT_GUI) || defined(UNIX)
    int		    force_next = FALSE;
#endif

    if (end_row > screen_Rows)		/* safety check */
	end_row = screen_Rows;
    if (end_col > screen_Columns)	/* safety check */
	end_col = screen_Columns;
    if (ScreenLines == NULL
	    || start_row >= end_row
	    || start_col >= end_col)	/* nothing to do */
	return;

    /* it's a "normal" terminal when not in a GUI or cterm */
    norm_term = (
#ifdef FEAT_GUI
	    !gui.in_use &&
#endif
			    t_colors <= 1);
    for (row = start_row; row < end_row; ++row)
    {
#ifdef FEAT_MBYTE
	if (has_mbyte
# ifdef FEAT_GUI
		&& !gui.in_use
# endif
	   )
	{
	    /* When drawing over the right halve of a double-wide char clear
	     * out the left halve.  When drawing over the left halve of a
	     * double wide-char clear out the right halve.  Only needed in a
	     * terminal. */
	    if (start_col > 0 && mb_fix_col(start_col, row) != start_col)
		screen_puts_len((char_u *)" ", 1, row, start_col - 1, 0);
	    if (end_col < screen_Columns && mb_fix_col(end_col, row) != end_col)
		screen_puts_len((char_u *)" ", 1, row, end_col, 0);
	}
#endif
	/*
	 * Try to use delete-line termcap code, when no attributes or in a
	 * "normal" terminal, where a bold/italic space is just a
	 * space.
	 */
	did_delete = FALSE;
	if (c2 == ' '
		&& end_col == Columns
		&& can_clear(T_CE)
		&& (attr == 0
		    || (norm_term
			&& attr <= HL_ALL
			&& ((attr & ~(HL_BOLD | HL_ITALIC)) == 0))))
	{
	    /*
	     * check if we really need to clear something
	     */
	    col = start_col;
	    if (c1 != ' ')			/* don't clear first char */
		++col;

	    off = LineOffset[row] + col;
	    end_off = LineOffset[row] + end_col;

	    /* skip blanks (used often, keep it fast!) */
#ifdef FEAT_MBYTE
	    if (enc_utf8)
		while (off < end_off && ScreenLines[off] == ' '
			  && ScreenAttrs[off] == 0 && ScreenLinesUC[off] == 0)
		    ++off;
	    else
#endif
		while (off < end_off && ScreenLines[off] == ' '
						     && ScreenAttrs[off] == 0)
		    ++off;
	    if (off < end_off)		/* something to be cleared */
	    {
		col = off - LineOffset[row];
		screen_stop_highlight();
		term_windgoto(row, col);/* clear rest of this screen line */
		out_str(T_CE);
		screen_start();		/* don't know where cursor is now */
		col = end_col - col;
		while (col--)		/* clear chars in ScreenLines */
		{
		    ScreenLines[off] = ' ';
#ifdef FEAT_MBYTE
		    if (enc_utf8)
			ScreenLinesUC[off] = 0;
#endif
		    ScreenAttrs[off] = 0;
		    ++off;
		}
	    }
	    did_delete = TRUE;		/* the chars are cleared now */
	}

	off = LineOffset[row] + start_col;
	c = c1;
	for (col = start_col; col < end_col; ++col)
	{
	    if (ScreenLines[off] != c
#ifdef FEAT_MBYTE
		    || (enc_utf8 && (int)ScreenLinesUC[off]
						       != (c >= 0x80 ? c : 0))
#endif
		    || ScreenAttrs[off] != attr
#if defined(FEAT_GUI) || defined(UNIX)
		    || force_next
#endif
		    )
	    {
#if defined(FEAT_GUI) || defined(UNIX)
		/* The bold trick may make a single row of pixels appear in
		 * the next character.  When a bold character is removed, the
		 * next character should be redrawn too.  This happens for our
		 * own GUI and for some xterms.  */
		if (
# ifdef FEAT_GUI
			gui.in_use
# endif
# if defined(FEAT_GUI) && defined(UNIX)
			||
# endif
# ifdef UNIX
			term_is_xterm
# endif
		   )
		{
		    if (ScreenLines[off] != ' '
			    && (ScreenAttrs[off] > HL_ALL
				|| ScreenAttrs[off] & HL_BOLD))
			force_next = TRUE;
		    else
			force_next = FALSE;
		}
#endif
		ScreenLines[off] = c;
#ifdef FEAT_MBYTE
		if (enc_utf8)
		{
		    if (c >= 0x80)
		    {
			ScreenLinesUC[off] = c;
			ScreenLinesC[0][off] = 0;
		    }
		    else
			ScreenLinesUC[off] = 0;
		}
#endif
		ScreenAttrs[off] = attr;
		if (!did_delete || c != ' ')
		    screen_char(off, row, col);
	    }
	    ++off;
	    if (col == start_col)
	    {
		if (did_delete)
		    break;
		c = c2;
	    }
	}
	if (end_col == Columns)
	    LineWraps[row] = FALSE;
	if (row == Rows - 1)		/* overwritten the command line */
	{
	    redraw_cmdline = TRUE;
	    if (c1 == ' ' && c2 == ' ')
		clear_cmdline = FALSE;	/* command line has been cleared */
	    if (start_col == 0)
		mode_displayed = FALSE; /* mode cleared or overwritten */
	}
    }
}

/*
 * Check if there should be a delay.  Used before clearing or redrawing the
 * screen or the command line.
 */
    void
check_for_delay(int check_msg_scroll)
{
    if ((emsg_on_display || (check_msg_scroll && msg_scroll))
	    && !did_wait_return
	    && emsg_silent == 0)
    {
	out_flush();
	ui_delay(1000L, TRUE);
	emsg_on_display = FALSE;
	if (check_msg_scroll)
	    msg_scroll = FALSE;
    }
}

/*
 * screen_valid -  allocate screen buffers if size changed
 *   If "doclear" is TRUE: clear screen if it has been resized.
 *	Returns TRUE if there is a valid screen to write to.
 *	Returns FALSE when starting up and screen not initialized yet.
 */
    int
screen_valid(int doclear)
{
    screenalloc(doclear);	   /* allocate screen buffers if size changed */
    return (ScreenLines != NULL);
}

/*
 * Resize the shell to Rows and Columns.
 * Allocate ScreenLines[] and associated items.
 *
 * There may be some time between setting Rows and Columns and (re)allocating
 * ScreenLines[].  This happens when starting up and when (manually) changing
 * the shell size.  Always use screen_Rows and screen_Columns to access items
 * in ScreenLines[].  Use Rows and Columns for positioning text etc. where the
 * final size of the shell is needed.
 */
    void
screenalloc(int doclear)
{
    int		    new_row, old_row;
#ifdef FEAT_GUI
    int		    old_Rows;
#endif
    win_T	    *wp;
    int		    outofmem = FALSE;
    int		    len;
    schar_T	    *new_ScreenLines;
#ifdef FEAT_MBYTE
    u8char_T	    *new_ScreenLinesUC = NULL;
    u8char_T	    *new_ScreenLinesC[MAX_MCO];
    schar_T	    *new_ScreenLines2 = NULL;
    int		    i;
#endif
    sattr_T	    *new_ScreenAttrs;
    unsigned	    *new_LineOffset;
    char_u	    *new_LineWraps;
#ifdef FEAT_WINDOWS
    short	    *new_TabPageIdxs;
    tabpage_T	    *tp;
#endif
    static int	    entered = FALSE;		/* avoid recursiveness */
    static int	    done_outofmem_msg = FALSE;	/* did outofmem message */
#ifdef FEAT_AUTOCMD
    int		    retry_count = 0;

retry:
#endif
    /*
     * Allocation of the screen buffers is done only when the size changes and
     * when Rows and Columns have been set and we have started doing full
     * screen stuff.
     */
    if ((ScreenLines != NULL
		&& Rows == screen_Rows
		&& Columns == screen_Columns
#ifdef FEAT_MBYTE
		&& enc_utf8 == (ScreenLinesUC != NULL)
		&& (enc_dbcs == DBCS_JPNU) == (ScreenLines2 != NULL)
		&& p_mco == Screen_mco
#endif
		)
	    || Rows == 0
	    || Columns == 0
	    || (!full_screen && ScreenLines == NULL))
	return;

    /*
     * It's possible that we produce an out-of-memory message below, which
     * will cause this function to be called again.  To break the loop, just
     * return here.
     */
    if (entered)
	return;
    entered = TRUE;

    /*
     * Note that the window sizes are updated before reallocating the arrays,
     * thus we must not redraw here!
     */
    ++RedrawingDisabled;

    win_new_shellsize();    /* fit the windows in the new sized shell */

    comp_col();		/* recompute columns for shown command and ruler */

    /*
     * We're changing the size of the screen.
     * - Allocate new arrays for ScreenLines and ScreenAttrs.
     * - Move lines from the old arrays into the new arrays, clear extra
     *	 lines (unless the screen is going to be cleared).
     * - Free the old arrays.
     *
     * If anything fails, make ScreenLines NULL, so we don't do anything!
     * Continuing with the old ScreenLines may result in a crash, because the
     * size is wrong.
     */
    FOR_ALL_TAB_WINDOWS(tp, wp)
	win_free_lsize(wp);
#ifdef FEAT_AUTOCMD
    if (aucmd_win != NULL)
	win_free_lsize(aucmd_win);
#endif

    new_ScreenLines = (schar_T *)lalloc((long_u)(
			      (Rows + 1) * Columns * sizeof(schar_T)), FALSE);
#ifdef FEAT_MBYTE
    vim_memset(new_ScreenLinesC, 0, sizeof(u8char_T *) * MAX_MCO);
    if (enc_utf8)
    {
	new_ScreenLinesUC = (u8char_T *)lalloc((long_u)(
			     (Rows + 1) * Columns * sizeof(u8char_T)), FALSE);
	for (i = 0; i < p_mco; ++i)
	    new_ScreenLinesC[i] = (u8char_T *)lalloc_clear((long_u)(
			     (Rows + 1) * Columns * sizeof(u8char_T)), FALSE);
    }
    if (enc_dbcs == DBCS_JPNU)
	new_ScreenLines2 = (schar_T *)lalloc((long_u)(
			     (Rows + 1) * Columns * sizeof(schar_T)), FALSE);
#endif
    new_ScreenAttrs = (sattr_T *)lalloc((long_u)(
			      (Rows + 1) * Columns * sizeof(sattr_T)), FALSE);
    new_LineOffset = (unsigned *)lalloc((long_u)(
					 Rows * sizeof(unsigned)), FALSE);
    new_LineWraps = (char_u *)lalloc((long_u)(Rows * sizeof(char_u)), FALSE);
#ifdef FEAT_WINDOWS
    new_TabPageIdxs = (short *)lalloc((long_u)(Columns * sizeof(short)), FALSE);
#endif

    FOR_ALL_TAB_WINDOWS(tp, wp)
    {
	if (win_alloc_lines(wp) == FAIL)
	{
	    outofmem = TRUE;
#ifdef FEAT_WINDOWS
	    goto give_up;
#endif
	}
    }
#ifdef FEAT_AUTOCMD
    if (aucmd_win != NULL && aucmd_win->w_lines == NULL
					&& win_alloc_lines(aucmd_win) == FAIL)
	outofmem = TRUE;
#endif
#ifdef FEAT_WINDOWS
give_up:
#endif

#ifdef FEAT_MBYTE
    for (i = 0; i < p_mco; ++i)
	if (new_ScreenLinesC[i] == NULL)
	    break;
#endif
    if (new_ScreenLines == NULL
#ifdef FEAT_MBYTE
	    || (enc_utf8 && (new_ScreenLinesUC == NULL || i != p_mco))
	    || (enc_dbcs == DBCS_JPNU && new_ScreenLines2 == NULL)
#endif
	    || new_ScreenAttrs == NULL
	    || new_LineOffset == NULL
	    || new_LineWraps == NULL
#ifdef FEAT_WINDOWS
	    || new_TabPageIdxs == NULL
#endif
	    || outofmem)
    {
	if (ScreenLines != NULL || !done_outofmem_msg)
	{
	    /* guess the size */
	    do_outofmem_msg((long_u)((Rows + 1) * Columns));

	    /* Remember we did this to avoid getting outofmem messages over
	     * and over again. */
	    done_outofmem_msg = TRUE;
	}
	vim_free(new_ScreenLines);
	new_ScreenLines = NULL;
#ifdef FEAT_MBYTE
	vim_free(new_ScreenLinesUC);
	new_ScreenLinesUC = NULL;
	for (i = 0; i < p_mco; ++i)
	{
	    vim_free(new_ScreenLinesC[i]);
	    new_ScreenLinesC[i] = NULL;
	}
	vim_free(new_ScreenLines2);
	new_ScreenLines2 = NULL;
#endif
	vim_free(new_ScreenAttrs);
	new_ScreenAttrs = NULL;
	vim_free(new_LineOffset);
	new_LineOffset = NULL;
	vim_free(new_LineWraps);
	new_LineWraps = NULL;
#ifdef FEAT_WINDOWS
	vim_free(new_TabPageIdxs);
	new_TabPageIdxs = NULL;
#endif
    }
    else
    {
	done_outofmem_msg = FALSE;

	for (new_row = 0; new_row < Rows; ++new_row)
	{
	    new_LineOffset[new_row] = new_row * Columns;
	    new_LineWraps[new_row] = FALSE;

	    /*
	     * If the screen is not going to be cleared, copy as much as
	     * possible from the old screen to the new one and clear the rest
	     * (used when resizing the window at the "--more--" prompt or when
	     * executing an external command, for the GUI).
	     */
	    if (!doclear)
	    {
		(void)vim_memset(new_ScreenLines + new_row * Columns,
				      ' ', (size_t)Columns * sizeof(schar_T));
#ifdef FEAT_MBYTE
		if (enc_utf8)
		{
		    (void)vim_memset(new_ScreenLinesUC + new_row * Columns,
				       0, (size_t)Columns * sizeof(u8char_T));
		    for (i = 0; i < p_mco; ++i)
			(void)vim_memset(new_ScreenLinesC[i]
							  + new_row * Columns,
				       0, (size_t)Columns * sizeof(u8char_T));
		}
		if (enc_dbcs == DBCS_JPNU)
		    (void)vim_memset(new_ScreenLines2 + new_row * Columns,
				       0, (size_t)Columns * sizeof(schar_T));
#endif
		(void)vim_memset(new_ScreenAttrs + new_row * Columns,
					0, (size_t)Columns * sizeof(sattr_T));
		old_row = new_row + (screen_Rows - Rows);
		if (old_row >= 0 && ScreenLines != NULL)
		{
		    if (screen_Columns < Columns)
			len = screen_Columns;
		    else
			len = Columns;
#ifdef FEAT_MBYTE
		    /* When switching to utf-8 don't copy characters, they
		     * may be invalid now.  Also when p_mco changes. */
		    if (!(enc_utf8 && ScreenLinesUC == NULL)
						       && p_mco == Screen_mco)
#endif
			mch_memmove(new_ScreenLines + new_LineOffset[new_row],
				ScreenLines + LineOffset[old_row],
				(size_t)len * sizeof(schar_T));
#ifdef FEAT_MBYTE
		    if (enc_utf8 && ScreenLinesUC != NULL
						       && p_mco == Screen_mco)
		    {
			mch_memmove(new_ScreenLinesUC + new_LineOffset[new_row],
				ScreenLinesUC + LineOffset[old_row],
				(size_t)len * sizeof(u8char_T));
			for (i = 0; i < p_mco; ++i)
			    mch_memmove(new_ScreenLinesC[i]
						    + new_LineOffset[new_row],
				ScreenLinesC[i] + LineOffset[old_row],
				(size_t)len * sizeof(u8char_T));
		    }
		    if (enc_dbcs == DBCS_JPNU && ScreenLines2 != NULL)
			mch_memmove(new_ScreenLines2 + new_LineOffset[new_row],
				ScreenLines2 + LineOffset[old_row],
				(size_t)len * sizeof(schar_T));
#endif
		    mch_memmove(new_ScreenAttrs + new_LineOffset[new_row],
			    ScreenAttrs + LineOffset[old_row],
			    (size_t)len * sizeof(sattr_T));
		}
	    }
	}
	/* Use the last line of the screen for the current line. */
	current_ScreenLine = new_ScreenLines + Rows * Columns;
    }

    free_screenlines();

    ScreenLines = new_ScreenLines;
#ifdef FEAT_MBYTE
    ScreenLinesUC = new_ScreenLinesUC;
    for (i = 0; i < p_mco; ++i)
	ScreenLinesC[i] = new_ScreenLinesC[i];
    Screen_mco = p_mco;
    ScreenLines2 = new_ScreenLines2;
#endif
    ScreenAttrs = new_ScreenAttrs;
    LineOffset = new_LineOffset;
    LineWraps = new_LineWraps;
#ifdef FEAT_WINDOWS
    TabPageIdxs = new_TabPageIdxs;
#endif

    /* It's important that screen_Rows and screen_Columns reflect the actual
     * size of ScreenLines[].  Set them before calling anything. */
#ifdef FEAT_GUI
    old_Rows = screen_Rows;
#endif
    screen_Rows = Rows;
    screen_Columns = Columns;

    must_redraw = CLEAR;	/* need to clear the screen later */
    if (doclear)
	screenclear2();

#ifdef FEAT_GUI
    else if (gui.in_use
	    && !gui.starting
	    && ScreenLines != NULL
	    && old_Rows != Rows)
    {
	(void)gui_redraw_block(0, 0, (int)Rows - 1, (int)Columns - 1, 0);
	/*
	 * Adjust the position of the cursor, for when executing an external
	 * command.
	 */
	if (msg_row >= Rows)		/* Rows got smaller */
	    msg_row = Rows - 1;		/* put cursor at last row */
	else if (Rows > old_Rows)	/* Rows got bigger */
	    msg_row += Rows - old_Rows; /* put cursor in same place */
	if (msg_col >= Columns)		/* Columns got smaller */
	    msg_col = Columns - 1;	/* put cursor at last column */
    }
#endif

    entered = FALSE;
    --RedrawingDisabled;

#ifdef FEAT_AUTOCMD
    /*
     * Do not apply autocommands more than 3 times to avoid an endless loop
     * in case applying autocommands always changes Rows or Columns.
     */
    if (starting == 0 && ++retry_count <= 3)
    {
	apply_autocmds(EVENT_VIMRESIZED, NULL, NULL, FALSE, curbuf);
	/* In rare cases, autocommands may have altered Rows or Columns,
	 * jump back to check if we need to allocate the screen again. */
	goto retry;
    }
#endif
}

    void
free_screenlines(void)
{
#ifdef FEAT_MBYTE
    int		i;

    vim_free(ScreenLinesUC);
    for (i = 0; i < Screen_mco; ++i)
	vim_free(ScreenLinesC[i]);
    vim_free(ScreenLines2);
#endif
    vim_free(ScreenLines);
    vim_free(ScreenAttrs);
    vim_free(LineOffset);
    vim_free(LineWraps);
#ifdef FEAT_WINDOWS
    vim_free(TabPageIdxs);
#endif
}

    void
screenclear(void)
{
    check_for_delay(FALSE);
    screenalloc(FALSE);	    /* allocate screen buffers if size changed */
    screenclear2();	    /* clear the screen */
}

    static void
screenclear2(void)
{
    int	    i;

    if (starting == NO_SCREEN || ScreenLines == NULL
#ifdef FEAT_GUI
	    || (gui.in_use && gui.starting)
#endif
	    )
	return;

#ifdef FEAT_GUI
    if (!gui.in_use)
#endif
	screen_attr = -1;	/* force setting the Normal colors */
    screen_stop_highlight();	/* don't want highlighting here */

#ifdef FEAT_CLIPBOARD
    /* disable selection without redrawing it */
    clip_scroll_selection(9999);
#endif

    /* blank out ScreenLines */
    for (i = 0; i < Rows; ++i)
    {
	lineclear(LineOffset[i], (int)Columns);
	LineWraps[i] = FALSE;
    }

    if (can_clear(T_CL))
    {
	out_str(T_CL);		/* clear the display */
	clear_cmdline = FALSE;
	mode_displayed = FALSE;
    }
    else
    {
	/* can't clear the screen, mark all chars with invalid attributes */
	for (i = 0; i < Rows; ++i)
	    lineinvalid(LineOffset[i], (int)Columns);
	clear_cmdline = TRUE;
    }

    screen_cleared = TRUE;	/* can use contents of ScreenLines now */

    win_rest_invalid(firstwin);
    redraw_cmdline = TRUE;
#ifdef FEAT_WINDOWS
    redraw_tabline = TRUE;
#endif
    if (must_redraw == CLEAR)	/* no need to clear again */
	must_redraw = NOT_VALID;
    compute_cmdrow();
    msg_row = cmdline_row;	/* put cursor on last line for messages */
    msg_col = 0;
    screen_start();		/* don't know where cursor is now */
    msg_scrolled = 0;		/* can't scroll back */
    msg_didany = FALSE;
    msg_didout = FALSE;
}

/*
 * Clear one line in ScreenLines.
 */
    static void
lineclear(unsigned off, int width)
{
    (void)vim_memset(ScreenLines + off, ' ', (size_t)width * sizeof(schar_T));
#ifdef FEAT_MBYTE
    if (enc_utf8)
	(void)vim_memset(ScreenLinesUC + off, 0,
					  (size_t)width * sizeof(u8char_T));
#endif
    (void)vim_memset(ScreenAttrs + off, 0, (size_t)width * sizeof(sattr_T));
}

/*
 * Mark one line in ScreenLines invalid by setting the attributes to an
 * invalid value.
 */
    static void
lineinvalid(unsigned off, int width)
{
    (void)vim_memset(ScreenAttrs + off, -1, (size_t)width * sizeof(sattr_T));
}

#ifdef FEAT_WINDOWS
/*
 * Copy part of a Screenline for vertically split window "wp".
 */
    static void
linecopy(int to, int from, win_T *wp)
{
    unsigned	off_to = LineOffset[to] + wp->w_wincol;
    unsigned	off_from = LineOffset[from] + wp->w_wincol;

    mch_memmove(ScreenLines + off_to, ScreenLines + off_from,
	    wp->w_width * sizeof(schar_T));
# ifdef FEAT_MBYTE
    if (enc_utf8)
    {
	int	i;

	mch_memmove(ScreenLinesUC + off_to, ScreenLinesUC + off_from,
		wp->w_width * sizeof(u8char_T));
	for (i = 0; i < p_mco; ++i)
	    mch_memmove(ScreenLinesC[i] + off_to, ScreenLinesC[i] + off_from,
		    wp->w_width * sizeof(u8char_T));
    }
    if (enc_dbcs == DBCS_JPNU)
	mch_memmove(ScreenLines2 + off_to, ScreenLines2 + off_from,
		wp->w_width * sizeof(schar_T));
# endif
    mch_memmove(ScreenAttrs + off_to, ScreenAttrs + off_from,
	    wp->w_width * sizeof(sattr_T));
}
#endif

/*
 * Return TRUE if clearing with term string "p" would work.
 * It can't work when the string is empty or it won't set the right background.
 */
    int
can_clear(char_u *p)
{
    return (*p != NUL && (t_colors <= 1
#ifdef FEAT_GUI
		|| gui.in_use
#endif
		|| cterm_normal_bg_color == 0 || *T_UT != NUL));
}

/*
 * Reset cursor position. Use whenever cursor was moved because of outputting
 * something directly to the screen (shell commands) or a terminal control
 * code.
 */
    void
screen_start(void)
{
    screen_cur_row = screen_cur_col = 9999;
}

/*
 * Move the cursor to position "row","col" in the screen.
 * This tries to find the most efficient way to move, minimizing the number of
 * characters sent to the terminal.
 */
    void
windgoto(int row, int col)
{
    sattr_T	    *p;
    int		    i;
    int		    plan;
    int		    cost;
    int		    wouldbe_col;
    int		    noinvcurs;
    char_u	    *bs;
    int		    goto_cost;
    int		    attr;

#define GOTO_COST   7	/* assume a term_windgoto() takes about 7 chars */
#define HIGHL_COST  5	/* assume unhighlight takes 5 chars */

#define PLAN_LE	    1
#define PLAN_CR	    2
#define PLAN_NL	    3
#define PLAN_WRITE  4
    /* Can't use ScreenLines unless initialized */
    if (ScreenLines == NULL)
	return;

    if (col != screen_cur_col || row != screen_cur_row)
    {
	/* Check for valid position. */
	if (row < 0)	/* window without text lines? */
	    row = 0;
	if (row >= screen_Rows)
	    row = screen_Rows - 1;
	if (col >= screen_Columns)
	    col = screen_Columns - 1;

	/* check if no cursor movement is allowed in highlight mode */
	if (screen_attr && *T_MS == NUL)
	    noinvcurs = HIGHL_COST;
	else
	    noinvcurs = 0;
	goto_cost = GOTO_COST + noinvcurs;

	/*
	 * Plan how to do the positioning:
	 * 1. Use CR to move it to column 0, same row.
	 * 2. Use T_LE to move it a few columns to the left.
	 * 3. Use NL to move a few lines down, column 0.
	 * 4. Move a few columns to the right with T_ND or by writing chars.
	 *
	 * Don't do this if the cursor went beyond the last column, the cursor
	 * position is unknown then (some terminals wrap, some don't )
	 *
	 * First check if the highlighting attributes allow us to write
	 * characters to move the cursor to the right.
	 */
	if (row >= screen_cur_row && screen_cur_col < Columns)
	{
	    /*
	     * If the cursor is in the same row, bigger col, we can use CR
	     * or T_LE.
	     */
	    bs = NULL;			    /* init for GCC */
	    attr = screen_attr;
	    if (row == screen_cur_row && col < screen_cur_col)
	    {
		/* "le" is preferred over "bc", because "bc" is obsolete */
		if (*T_LE)
		    bs = T_LE;		    /* "cursor left" */
		else
		    bs = T_BC;		    /* "backspace character (old) */
		if (*bs)
		    cost = (screen_cur_col - col) * (int)STRLEN(bs);
		else
		    cost = 999;
		if (col + 1 < cost)	    /* using CR is less characters */
		{
		    plan = PLAN_CR;
		    wouldbe_col = 0;
		    cost = 1;		    /* CR is just one character */
		}
		else
		{
		    plan = PLAN_LE;
		    wouldbe_col = col;
		}
		if (noinvcurs)		    /* will stop highlighting */
		{
		    cost += noinvcurs;
		    attr = 0;
		}
	    }

	    /*
	     * If the cursor is above where we want to be, we can use CR LF.
	     */
	    else if (row > screen_cur_row)
	    {
		plan = PLAN_NL;
		wouldbe_col = 0;
		cost = (row - screen_cur_row) * 2;  /* CR LF */
		if (noinvcurs)		    /* will stop highlighting */
		{
		    cost += noinvcurs;
		    attr = 0;
		}
	    }

	    /*
	     * If the cursor is in the same row, smaller col, just use write.
	     */
	    else
	    {
		plan = PLAN_WRITE;
		wouldbe_col = screen_cur_col;
		cost = 0;
	    }

	    /*
	     * Check if any characters that need to be written have the
	     * correct attributes.  Also avoid UTF-8 characters.
	     */
	    i = col - wouldbe_col;
	    if (i > 0)
		cost += i;
	    if (cost < goto_cost && i > 0)
	    {
		/*
		 * Check if the attributes are correct without additionally
		 * stopping highlighting.
		 */
		p = ScreenAttrs + LineOffset[row] + wouldbe_col;
		while (i && *p++ == attr)
		    --i;
		if (i != 0)
		{
		    /*
		     * Try if it works when highlighting is stopped here.
		     */
		    if (*--p == 0)
		    {
			cost += noinvcurs;
			while (i && *p++ == 0)
			    --i;
		    }
		    if (i != 0)
			cost = 999;	/* different attributes, don't do it */
		}
#ifdef FEAT_MBYTE
		if (enc_utf8)
		{
		    /* Don't use an UTF-8 char for positioning, it's slow. */
		    for (i = wouldbe_col; i < col; ++i)
			if (ScreenLinesUC[LineOffset[row] + i] != 0)
			{
			    cost = 999;
			    break;
			}
		}
#endif
	    }

	    /*
	     * We can do it without term_windgoto()!
	     */
	    if (cost < goto_cost)
	    {
		if (plan == PLAN_LE)
		{
		    if (noinvcurs)
			screen_stop_highlight();
		    while (screen_cur_col > col)
		    {
			out_str(bs);
			--screen_cur_col;
		    }
		}
		else if (plan == PLAN_CR)
		{
		    if (noinvcurs)
			screen_stop_highlight();
		    out_char('\r');
		    screen_cur_col = 0;
		}
		else if (plan == PLAN_NL)
		{
		    if (noinvcurs)
			screen_stop_highlight();
		    while (screen_cur_row < row)
		    {
			out_char('\n');
			++screen_cur_row;
		    }
		    screen_cur_col = 0;
		}

		i = col - screen_cur_col;
		if (i > 0)
		{
		    /*
		     * Use cursor-right if it's one character only.  Avoids
		     * removing a line of pixels from the last bold char, when
		     * using the bold trick in the GUI.
		     */
		    if (T_ND[0] != NUL && T_ND[1] == NUL)
		    {
			while (i-- > 0)
			    out_char(*T_ND);
		    }
		    else
		    {
			int	off;

			off = LineOffset[row] + screen_cur_col;
			while (i-- > 0)
			{
			    if (ScreenAttrs[off] != screen_attr)
				screen_stop_highlight();
#ifdef FEAT_MBYTE
			    out_flush_check();
#endif
			    out_char(ScreenLines[off]);
#ifdef FEAT_MBYTE
			    if (enc_dbcs == DBCS_JPNU
						  && ScreenLines[off] == 0x8e)
				out_char(ScreenLines2[off]);
#endif
			    ++off;
			}
		    }
		}
	    }
	}
	else
	    cost = 999;

	if (cost >= goto_cost)
	{
	    if (noinvcurs)
		screen_stop_highlight();
	    if (row == screen_cur_row && (col > screen_cur_col)
							     && *T_CRI != NUL)
		term_cursor_right(col - screen_cur_col);
	    else
		term_windgoto(row, col);
	}
	screen_cur_row = row;
	screen_cur_col = col;
    }
}

/*
 * Set cursor to its position in the current window.
 */
    void
setcursor(void)
{
    if (redrawing())
    {
	validate_cursor();
	windgoto(W_WINROW(curwin) + curwin->w_wrow,
		W_WINCOL(curwin) + (
#ifdef FEAT_RIGHTLEFT
		/* With 'rightleft' set and the cursor on a double-wide
		 * character, position it on the leftmost column. */
		curwin->w_p_rl ? ((int)W_WIDTH(curwin) - curwin->w_wcol - (
# ifdef FEAT_MBYTE
			(has_mbyte
			   && (*mb_ptr2cells)(ml_get_cursor()) == 2
			   && vim_isprintc(gchar_cursor())) ? 2 :
# endif
			1)) :
#endif
							    curwin->w_wcol));
    }
}


/*
 * insert 'line_count' lines at 'row' in window 'wp'
 * if 'invalid' is TRUE the wp->w_lines[].wl_lnum is invalidated.
 * if 'mayclear' is TRUE the screen will be cleared if it is faster than
 * scrolling.
 * Returns FAIL if the lines are not inserted, OK for success.
 */
    int
win_ins_lines(
    win_T	*wp,
    int		row,
    int		line_count,
    int		invalid,
    int		mayclear)
{
    int		did_delete;
    int		nextrow;
    int		lastrow;
    int		retval;

    if (invalid)
	wp->w_lines_valid = 0;

    if (wp->w_height < 5)
	return FAIL;

    if (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, FALSE);
    if (retval != MAYBE)
	return retval;

    /*
     * If there is a next window or a status line, we first try to delete the
     * lines at the bottom to avoid messing what is after the window.
     * If this fails and there are following windows, don't do anything to avoid
     * messing up those windows, better just redraw.
     */
    did_delete = FALSE;
#ifdef FEAT_WINDOWS
    if (wp->w_next != NULL || wp->w_status_height)
    {
	if (screen_del_lines(0, W_WINROW(wp) + wp->w_height - line_count,
				    line_count, (int)Rows, FALSE, NULL) == OK)
	    did_delete = TRUE;
	else if (wp->w_next)
	    return FAIL;
    }
#endif
    /*
     * if no lines deleted, blank the lines that will end up below the window
     */
    if (!did_delete)
    {
#ifdef FEAT_WINDOWS
	wp->w_redr_status = TRUE;
#endif
	redraw_cmdline = TRUE;
	nextrow = W_WINROW(wp) + wp->w_height + W_STATUS_HEIGHT(wp);
	lastrow = nextrow + line_count;
	if (lastrow > Rows)
	    lastrow = Rows;
	screen_fill(nextrow - line_count, lastrow - line_count,
		  W_WINCOL(wp), (int)W_ENDCOL(wp),
		  ' ', ' ', 0);
    }

    if (screen_ins_lines(0, W_WINROW(wp) + row, line_count, (int)Rows, NULL)
								      == FAIL)
    {
	    /* deletion will have messed up other windows */
	if (did_delete)
	{
#ifdef FEAT_WINDOWS
	    wp->w_redr_status = TRUE;
#endif
	    win_rest_invalid(W_NEXT(wp));
	}
	return FAIL;
    }

    return OK;
}

/*
 * delete "line_count" window lines at "row" in window "wp"
 * If "invalid" is TRUE curwin->w_lines[] is invalidated.
 * If "mayclear" is TRUE the screen will be cleared if it is faster than
 * scrolling
 * Return OK for success, FAIL if the lines are not deleted.
 */
    int
win_del_lines(
    win_T	*wp,
    int		row,
    int		line_count,
    int		invalid,
    int		mayclear)
{
    int		retval;

    if (invalid)
	wp->w_lines_valid = 0;

    if (line_count > wp->w_height - row)
	line_count = wp->w_height - row;

    retval = win_do_lines(wp, row, line_count, mayclear, TRUE);
    if (retval != MAYBE)
	return retval;

    if (screen_del_lines(0, W_WINROW(wp) + row, line_count,
					      (int)Rows, FALSE, NULL) == FAIL)
	return FAIL;

#ifdef FEAT_WINDOWS
    /*
     * If there are windows or status lines below, try to put them at the
     * correct place. If we can't do that, they have to be redrawn.
     */
    if (wp->w_next || wp->w_status_height || cmdline_row < Rows - 1)
    {
	if (screen_ins_lines(0, W_WINROW(wp) + wp->w_height - line_count,
					 line_count, (int)Rows, NULL) == FAIL)
	{
	    wp->w_redr_status = TRUE;
	    win_rest_invalid(wp->w_next);
	}
    }
    /*
     * If this is the last window and there is no status line, redraw the
     * command line later.
     */
    else
#endif
	redraw_cmdline = TRUE;
    return OK;
}

/*
 * Common code for win_ins_lines() and win_del_lines().
 * Returns OK or FAIL when the work has been done.
 * Returns MAYBE when not finished yet.
 */
    static int
win_do_lines(
    win_T	*wp,
    int		row,
    int		line_count,
    int		mayclear,
    int		del)
{
    int		retval;

    if (!redrawing() || line_count <= 0)
	return FAIL;

    /* only a few lines left: redraw is faster */
    if (mayclear && Rows - line_count < 5
#ifdef FEAT_WINDOWS
	    && wp->w_width == Columns
#endif
	    )
    {
	screenclear();	    /* will set wp->w_lines_valid to 0 */
	return FAIL;
    }

    /*
     * Delete all remaining lines
     */
    if (row + line_count >= wp->w_height)
    {
	screen_fill(W_WINROW(wp) + row, W_WINROW(wp) + wp->w_height,
		W_WINCOL(wp), (int)W_ENDCOL(wp),
		' ', ' ', 0);
	return OK;
    }

    /*
     * when scrolling, the message on the command line should be cleared,
     * otherwise it will stay there forever.
     */
    clear_cmdline = TRUE;

    /*
     * If the terminal can set a scroll region, use that.
     * Always do this in a vertically split window.  This will redraw from
     * ScreenLines[] when t_CV isn't defined.  That's faster than using
     * win_line().
     * Don't use a scroll region when we are going to redraw the text, writing
     * a character in the lower right corner of the scroll region may cause a
     * scroll-up .
     */
    if (scroll_region
#ifdef FEAT_WINDOWS
	    || W_WIDTH(wp) != Columns
#endif
	    )
    {
#ifdef FEAT_WINDOWS
	if (scroll_region && (wp->w_width == Columns || *T_CSV != NUL))
#endif
	    scroll_region_set(wp, row);
	if (del)
	    retval = screen_del_lines(W_WINROW(wp) + row, 0, line_count,
					       wp->w_height - row, FALSE, wp);
	else
	    retval = screen_ins_lines(W_WINROW(wp) + row, 0, line_count,
						      wp->w_height - row, wp);
#ifdef FEAT_WINDOWS
	if (scroll_region && (wp->w_width == Columns || *T_CSV != NUL))
#endif
	    scroll_region_reset();
	return retval;
    }

#ifdef FEAT_WINDOWS
    if (wp->w_next != NULL && p_tf) /* don't delete/insert on fast terminal */
	return FAIL;
#endif

    return MAYBE;
}

/*
 * window 'wp' and everything after it is messed up, mark it for redraw
 */
    static void
win_rest_invalid(win_T *wp)
{
#ifdef FEAT_WINDOWS
    while (wp != NULL)
#else
    if (wp != NULL)
#endif
    {
	redraw_win_later(wp, NOT_VALID);
#ifdef FEAT_WINDOWS
	wp->w_redr_status = TRUE;
	wp = wp->w_next;
#endif
    }
    redraw_cmdline = TRUE;
}

/*
 * The rest of the routines in this file perform screen manipulations. The
 * given operation is performed physically on the screen. The corresponding
 * change is also made to the internal screen image. In this way, the editor
 * anticipates the effect of editing changes on the appearance of the screen.
 * That way, when we call screenupdate a complete redraw isn't usually
 * necessary. Another advantage is that we can keep adding code to anticipate
 * screen changes, and in the meantime, everything still works.
 */

/*
 * types for inserting or deleting lines
 */
#define USE_T_CAL   1
#define USE_T_CDL   2
#define USE_T_AL    3
#define USE_T_CE    4
#define USE_T_DL    5
#define USE_T_SR    6
#define USE_NL	    7
#define USE_T_CD    8
#define USE_REDRAW  9

/*
 * insert lines on the screen and update ScreenLines[]
 * 'end' is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used 'off' is the offset from the top for the region.
 * 'row' and 'end' are relative to the start of the region.
 *
 * return FAIL for failure, OK for success.
 */
    int
screen_ins_lines(
    int		off,
    int		row,
    int		line_count,
    int		end,
    win_T	*wp)	    /* NULL or window to use width from */
{
    int		i;
    int		j;
    unsigned	temp;
    int		cursor_row;
    int		type;
    int		result_empty;
    int		can_ce = can_clear(T_CE);

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     */
    if (!screen_valid(TRUE) || line_count <= 0 || line_count > p_ttyscroll)
	return FAIL;

    /*
     * There are seven ways to insert lines:
     * 0. When in a vertically split window and t_CV isn't set, redraw the
     *    characters from ScreenLines[].
     * 1. Use T_CD (clear to end of display) if it exists and the result of
     *	  the insert is just empty lines
     * 2. Use T_CAL (insert multiple lines) if it exists and T_AL is not
     *	  present or line_count > 1. It looks better if we do all the inserts
     *	  at once.
     * 3. Use T_CDL (delete multiple lines) if it exists and the result of the
     *	  insert is just empty lines and T_CE is not present or line_count >
     *	  1.
     * 4. Use T_AL (insert line) if it exists.
     * 5. Use T_CE (erase line) if it exists and the result of the insert is
     *	  just empty lines.
     * 6. Use T_DL (delete line) if it exists and the result of the insert is
     *	  just empty lines.
     * 7. Use T_SR (scroll reverse) if it exists and inserting at row 0 and
     *	  the 'da' flag is not set or we have clear line capability.
     * 8. redraw the characters from ScreenLines[].
     *
     * Careful: In a hpterm scroll reverse doesn't work as expected, it moves
     * the scrollbar for the window. It does have insert line, use that if it
     * exists.
     */
    result_empty = (row + line_count >= end);
#ifdef FEAT_WINDOWS
    if (wp != NULL && wp->w_width != Columns && *T_CSV == NUL)
	type = USE_REDRAW;
    else
#endif
    if (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
    else if (*T_CAL != NUL && (line_count > 1 || *T_AL == NUL))
	type = USE_T_CAL;
    else if (*T_CDL != NUL && result_empty && (line_count > 1 || !can_ce))
	type = USE_T_CDL;
    else if (*T_AL != NUL)
	type = USE_T_AL;
    else if (can_ce && result_empty)
	type = USE_T_CE;
    else if (*T_DL != NUL && result_empty)
	type = USE_T_DL;
    else if (*T_SR != NUL && row == 0 && (*T_DA == NUL || can_ce))
	type = USE_T_SR;
    else
	return FAIL;

    /*
     * For clearing the lines screen_del_lines() is used. This will also take
     * care of t_db if necessary.
     */
    if (type == USE_T_CD || type == USE_T_CDL ||
					 type == USE_T_CE || type == USE_T_DL)
	return screen_del_lines(off, row, line_count, end, FALSE, wp);

    /*
     * If text is retained below the screen, first clear or delete as many
     * lines at the bottom of the window as are about to be inserted so that
     * the deleted lines won't later surface during a screen_del_lines.
     */
    if (*T_DB)
	screen_del_lines(off, end - line_count, line_count, end, FALSE, wp);

#ifdef FEAT_CLIPBOARD
    /* Remove a modeless selection when inserting lines halfway the screen
     * or not the full width of the screen. */
    if (off + row > 0
# ifdef FEAT_WINDOWS
	    || (wp != NULL && wp->w_width != Columns)
# endif
       )
	clip_clear_selection(&clip_star);
    else
	clip_scroll_selection(-line_count);
#endif

#ifdef FEAT_GUI
    /* Don't update the GUI cursor here, ScreenLines[] is invalid until the
     * scrolling is actually carried out. */
    gui_dont_update_cursor();
#endif

    if (*T_CCS != NUL)	   /* cursor relative to region */
	cursor_row = row;
    else
	cursor_row = row + off;

    /*
     * Shift LineOffset[] line_count down to reflect the inserted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
#ifdef FEAT_WINDOWS
	if (wp != NULL && wp->w_width != Columns)
	{
	    /* need to copy part of a line */
	    j = end - 1 - i;
	    while ((j -= line_count) >= row)
		linecopy(j + line_count, j, wp);
	    j += line_count;
	    if (can_clear((char_u *)" "))
		lineclear(LineOffset[j] + wp->w_wincol, wp->w_width);
	    else
		lineinvalid(LineOffset[j] + wp->w_wincol, wp->w_width);
	    LineWraps[j] = FALSE;
	}
	else
#endif
	{
	    j = end - 1 - i;
	    temp = LineOffset[j];
	    while ((j -= line_count) >= row)
	    {
		LineOffset[j + line_count] = LineOffset[j];
		LineWraps[j + line_count] = LineWraps[j];
	    }
	    LineOffset[j + line_count] = temp;
	    LineWraps[j + line_count] = FALSE;
	    if (can_clear((char_u *)" "))
		lineclear(temp, (int)Columns);
	    else
		lineinvalid(temp, (int)Columns);
	}
    }

    screen_stop_highlight();
    windgoto(cursor_row, 0);

#ifdef FEAT_WINDOWS
    /* redraw the characters */
    if (type == USE_REDRAW)
	redraw_block(row, end, wp);
    else
#endif
	if (type == USE_T_CAL)
    {
	term_append_lines(line_count);
	screen_start();		/* don't know where cursor is now */
    }
    else
    {
	for (i = 0; i < line_count; i++)
	{
	    if (type == USE_T_AL)
	    {
		if (i && cursor_row != 0)
		    windgoto(cursor_row, 0);
		out_str(T_AL);
	    }
	    else  /* type == USE_T_SR */
		out_str(T_SR);
	    screen_start();	    /* don't know where cursor is now */
	}
    }

    /*
     * With scroll-reverse and 'da' flag set we need to clear the lines that
     * have been scrolled down into the region.
     */
    if (type == USE_T_SR && *T_DA)
    {
	for (i = 0; i < line_count; ++i)
	{
	    windgoto(off + i, 0);
	    out_str(T_CE);
	    screen_start();	    /* don't know where cursor is now */
	}
    }

#ifdef FEAT_GUI
    gui_can_update_cursor();
    if (gui.in_use)
	out_flush();	/* always flush after a scroll */
#endif
    return OK;
}

/*
 * delete lines on the screen and update ScreenLines[]
 * 'end' is the line after the scrolled part. Normally it is Rows.
 * When scrolling region used 'off' is the offset from the top for the region.
 * 'row' and 'end' are relative to the start of the region.
 *
 * Return OK for success, FAIL if the lines are not deleted.
 */
    int
screen_del_lines(
    int		off,
    int		row,
    int		line_count,
    int		end,
    int		force,		/* even when line_count > p_ttyscroll */
    win_T	*wp UNUSED)	/* NULL or window to use width from */
{
    int		j;
    int		i;
    unsigned	temp;
    int		cursor_row;
    int		cursor_end;
    int		result_empty;	/* result is empty until end of region */
    int		can_delete;	/* deleting line codes can be used */
    int		type;

    /*
     * FAIL if
     * - there is no valid screen
     * - the screen has to be redrawn completely
     * - the line count is less than one
     * - the line count is more than 'ttyscroll'
     */
    if (!screen_valid(TRUE) || line_count <= 0 ||
					 (!force && line_count > p_ttyscroll))
	return FAIL;

    /*
     * Check if the rest of the current region will become empty.
     */
    result_empty = row + line_count >= end;

    /*
     * We can delete lines only when 'db' flag not set or when 'ce' option
     * available.
     */
    can_delete = (*T_DB == NUL || can_clear(T_CE));

    /*
     * There are six ways to delete lines:
     * 0. When in a vertically split window and t_CV isn't set, redraw the
     *    characters from ScreenLines[].
     * 1. Use T_CD if it exists and the result is empty.
     * 2. Use newlines if row == 0 and count == 1 or T_CDL does not exist.
     * 3. Use T_CDL (delete multiple lines) if it exists and line_count > 1 or
     *	  none of the other ways work.
     * 4. Use T_CE (erase line) if the result is empty.
     * 5. Use T_DL (delete line) if it exists.
     * 6. redraw the characters from ScreenLines[].
     */
#ifdef FEAT_WINDOWS
    if (wp != NULL && wp->w_width != Columns && *T_CSV == NUL)
	type = USE_REDRAW;
    else
#endif
    if (can_clear(T_CD) && result_empty)
	type = USE_T_CD;
#if defined(__BEOS__) && defined(BEOS_DR8)
    /*
     * USE_NL does not seem to work in Terminal of DR8 so we set T_DB="" in
     * its internal termcap... this works okay for tests which test *T_DB !=
     * NUL.  It has the disadvantage that the user cannot use any :set t_*
     * command to get T_DB (back) to empty_option, only :set term=... will do
     * the trick...
     * Anyway, this hack will hopefully go away with the next OS release.
     * (Olaf Seibert)
     */
    else if (row == 0 && T_DB == empty_option
					&& (line_count == 1 || *T_CDL == NUL))
#else
    else if (row == 0 && (
#ifndef AMIGA
	/* On the Amiga, somehow '\n' on the last line doesn't always scroll
	 * up, so use delete-line command */
			    line_count == 1 ||
#endif
						*T_CDL == NUL))
#endif
	type = USE_NL;
    else if (*T_CDL != NUL && line_count > 1 && can_delete)
	type = USE_T_CDL;
    else if (can_clear(T_CE) && result_empty
#ifdef FEAT_WINDOWS
	    && (wp == NULL || wp->w_width == Columns)
#endif
	    )
	type = USE_T_CE;
    else if (*T_DL != NUL && can_delete)
	type = USE_T_DL;
    else if (*T_CDL != NUL && can_delete)
	type = USE_T_CDL;
    else
	return FAIL;

#ifdef FEAT_CLIPBOARD
    /* Remove a modeless selection when deleting lines halfway the screen or
     * not the full width of the screen. */
    if (off + row > 0
# ifdef FEAT_WINDOWS
	    || (wp != NULL && wp->w_width != Columns)
# endif
       )
	clip_clear_selection(&clip_star);
    else
	clip_scroll_selection(line_count);
#endif

#ifdef FEAT_GUI
    /* Don't update the GUI cursor here, ScreenLines[] is invalid until the
     * scrolling is actually carried out. */
    gui_dont_update_cursor();
#endif

    if (*T_CCS != NUL)	    /* cursor relative to region */
    {
	cursor_row = row;
	cursor_end = end;
    }
    else
    {
	cursor_row = row + off;
	cursor_end = end + off;
    }

    /*
     * Now shift LineOffset[] line_count up to reflect the deleted lines.
     * Clear the inserted lines in ScreenLines[].
     */
    row += off;
    end += off;
    for (i = 0; i < line_count; ++i)
    {
#ifdef FEAT_WINDOWS
	if (wp != NULL && wp->w_width != Columns)
	{
	    /* need to copy part of a line */
	    j = row + i;
	    while ((j += line_count) <= end - 1)
		linecopy(j - line_count, j, wp);
	    j -= line_count;
	    if (can_clear((char_u *)" "))
		lineclear(LineOffset[j] + wp->w_wincol, wp->w_width);
	    else
		lineinvalid(LineOffset[j] + wp->w_wincol, wp->w_width);
	    LineWraps[j] = FALSE;
	}
	else
#endif
	{
	    /* whole width, moving the line pointers is faster */
	    j = row + i;
	    temp = LineOffset[j];
	    while ((j += line_count) <= end - 1)
	    {
		LineOffset[j - line_count] = LineOffset[j];
		LineWraps[j - line_count] = LineWraps[j];
	    }
	    LineOffset[j - line_count] = temp;
	    LineWraps[j - line_count] = FALSE;
	    if (can_clear((char_u *)" "))
		lineclear(temp, (int)Columns);
	    else
		lineinvalid(temp, (int)Columns);
	}
    }

    screen_stop_highlight();

#ifdef FEAT_WINDOWS
    /* redraw the characters */
    if (type == USE_REDRAW)
	redraw_block(row, end, wp);
    else
#endif
	if (type == USE_T_CD)	/* delete the lines */
    {
	windgoto(cursor_row, 0);
	out_str(T_CD);
	screen_start();			/* don't know where cursor is now */
    }
    else if (type == USE_T_CDL)
    {
	windgoto(cursor_row, 0);
	term_delete_lines(line_count);
	screen_start();			/* don't know where cursor is now */
    }
    /*
     * Deleting lines at top of the screen or scroll region: Just scroll
     * the whole screen (scroll region) up by outputting newlines on the
     * last line.
     */
    else if (type == USE_NL)
    {
	windgoto(cursor_end - 1, 0);
	for (i = line_count; --i >= 0; )
	    out_char('\n');		/* cursor will remain on same line */
    }
    else
    {
	for (i = line_count; --i >= 0; )
	{
	    if (type == USE_T_DL)
	    {
		windgoto(cursor_row, 0);
		out_str(T_DL);		/* delete a line */
	    }
	    else /* type == USE_T_CE */
	    {
		windgoto(cursor_row + i, 0);
		out_str(T_CE);		/* erase a line */
	    }
	    screen_start();		/* don't know where cursor is now */
	}
    }

    /*
     * If the 'db' flag is set, we need to clear the lines that have been
     * scrolled up at the bottom of the region.
     */
    if (*T_DB && (type == USE_T_DL || type == USE_T_CDL))
    {
	for (i = line_count; i > 0; --i)
	{
	    windgoto(cursor_end - i, 0);
	    out_str(T_CE);		/* erase a line */
	    screen_start();		/* don't know where cursor is now */
	}
    }

#ifdef FEAT_GUI
    gui_can_update_cursor();
    if (gui.in_use)
	out_flush();	/* always flush after a scroll */
#endif

    return OK;
}

/*
 * show the current mode and ruler
 *
 * If clear_cmdline is TRUE, clear the rest of the cmdline.
 * If clear_cmdline is FALSE there may be a message there that needs to be
 * cleared only if a mode is shown.
 * Return the length of the message (0 if no message).
 */
    int
showmode(void)
{
    int		need_clear;
    int		length = 0;
    int		do_mode;
    int		attr;
    int		nwr_save;
#ifdef FEAT_INS_EXPAND
    int		sub_attr;
#endif

    do_mode = ((p_smd && msg_silent == 0)
	    && ((State & INSERT)
		|| restart_edit
		|| VIsual_active));
    if (do_mode || Recording)
    {
	/*
	 * Don't show mode right now, when not redrawing or inside a mapping.
	 * Call char_avail() only when we are going to show something, because
	 * it takes a bit of time.
	 */
	if (!redrawing() || (char_avail() && !KeyTyped) || msg_silent != 0)
	{
	    redraw_cmdline = TRUE;		/* show mode later */
	    return 0;
	}

	nwr_save = need_wait_return;

	/* wait a bit before overwriting an important message */
	check_for_delay(FALSE);

	/* if the cmdline is more than one line high, erase top lines */
	need_clear = clear_cmdline;
	if (clear_cmdline && cmdline_row < Rows - 1)
	    msg_clr_cmdline();			/* will reset clear_cmdline */

	/* Position on the last line in the window, column 0 */
	msg_pos_mode();
	cursor_off();
	attr = hl_attr(HLF_CM);			/* Highlight mode */
	if (do_mode)
	{
	    MSG_PUTS_ATTR("--", attr);
#if defined(FEAT_XIM)
	    if (
# ifdef FEAT_GUI_GTK
		    preedit_get_status()
# else
		    im_get_status()
# endif
	       )
# ifdef FEAT_GUI_GTK /* most of the time, it's not XIM being used */
		MSG_PUTS_ATTR(" IM", attr);
# else
		MSG_PUTS_ATTR(" XIM", attr);
# endif
#endif
#if defined(FEAT_HANGULIN) && defined(FEAT_GUI)
	    if (gui.in_use)
	    {
		if (hangul_input_state_get())
		{
		    /* HANGUL */
		    if (enc_utf8)
			MSG_PUTS_ATTR(" \355\225\234\352\270\200", attr);
		    else
			MSG_PUTS_ATTR(" \307\321\261\333", attr);
		}
	    }
#endif
#ifdef FEAT_INS_EXPAND
	    /* CTRL-X in Insert mode */
	    if (edit_submode != NULL && !shortmess(SHM_COMPLETIONMENU))
	    {
		/* These messages can get long, avoid a wrap in a narrow
		 * window.  Prefer showing edit_submode_extra. */
		length = (Rows - msg_row) * Columns - 3;
		if (edit_submode_extra != NULL)
		    length -= vim_strsize(edit_submode_extra);
		if (length > 0)
		{
		    if (edit_submode_pre != NULL)
			length -= vim_strsize(edit_submode_pre);
		    if (length - vim_strsize(edit_submode) > 0)
		    {
			if (edit_submode_pre != NULL)
			    msg_puts_attr(edit_submode_pre, attr);
			msg_puts_attr(edit_submode, attr);
		    }
		    if (edit_submode_extra != NULL)
		    {
			MSG_PUTS_ATTR(" ", attr);  /* add a space in between */
			if ((int)edit_submode_highl < (int)HLF_COUNT)
			    sub_attr = hl_attr(edit_submode_highl);
			else
			    sub_attr = attr;
			msg_puts_attr(edit_submode_extra, sub_attr);
		    }
		}
		length = 0;
	    }
	    else
#endif
	    {
#ifdef FEAT_VREPLACE
		if (State & VREPLACE_FLAG)
		    MSG_PUTS_ATTR(_(" VREPLACE"), attr);
		else
#endif
		    if (State & REPLACE_FLAG)
		    MSG_PUTS_ATTR(_(" REPLACE"), attr);
		else if (State & INSERT)
		{
#ifdef FEAT_RIGHTLEFT
		    if (p_ri)
			MSG_PUTS_ATTR(_(" REVERSE"), attr);
#endif
		    MSG_PUTS_ATTR(_(" INSERT"), attr);
		}
		else if (restart_edit == 'I')
		    MSG_PUTS_ATTR(_(" (insert)"), attr);
		else if (restart_edit == 'R')
		    MSG_PUTS_ATTR(_(" (replace)"), attr);
		else if (restart_edit == 'V')
		    MSG_PUTS_ATTR(_(" (vreplace)"), attr);
#ifdef FEAT_RIGHTLEFT
		if (p_hkmap)
		    MSG_PUTS_ATTR(_(" Hebrew"), attr);
# ifdef FEAT_FKMAP
		if (p_fkmap)
		    MSG_PUTS_ATTR(farsi_text_5, attr);
# endif
#endif
#ifdef FEAT_KEYMAP
		if (State & LANGMAP)
		{
# ifdef FEAT_ARABIC
		    if (curwin->w_p_arab)
			MSG_PUTS_ATTR(_(" Arabic"), attr);
		    else
# endif
			MSG_PUTS_ATTR(_(" (lang)"), attr);
		}
#endif
		if ((State & INSERT) && p_paste)
		    MSG_PUTS_ATTR(_(" (paste)"), attr);

		if (VIsual_active)
		{
		    char *p;

		    /* Don't concatenate separate words to avoid translation
		     * problems. */
		    switch ((VIsual_select ? 4 : 0)
			    + (VIsual_mode == Ctrl_V) * 2
			    + (VIsual_mode == 'V'))
		    {
			case 0:	p = N_(" VISUAL"); break;
			case 1: p = N_(" VISUAL LINE"); break;
			case 2: p = N_(" VISUAL BLOCK"); break;
			case 4: p = N_(" SELECT"); break;
			case 5: p = N_(" SELECT LINE"); break;
			default: p = N_(" SELECT BLOCK"); break;
		    }
		    MSG_PUTS_ATTR(_(p), attr);
		}
		MSG_PUTS_ATTR(" --", attr);
	    }

	    need_clear = TRUE;
	}
	if (Recording
#ifdef FEAT_INS_EXPAND
		&& edit_submode == NULL	    /* otherwise it gets too long */
#endif
		)
	{
	    recording_mode(attr);
	    need_clear = TRUE;
	}

	mode_displayed = TRUE;
	if (need_clear || clear_cmdline)
	    msg_clr_eos();
	msg_didout = FALSE;		/* overwrite this message */
	length = msg_col;
	msg_col = 0;
	need_wait_return = nwr_save;	/* never ask for hit-return for this */
    }
    else if (clear_cmdline && msg_silent == 0)
	/* Clear the whole command line.  Will reset "clear_cmdline". */
	msg_clr_cmdline();

#ifdef FEAT_CMDL_INFO
    /* In Visual mode the size of the selected area must be redrawn. */
    if (VIsual_active)
	clear_showcmd();

    /* If the last window has no status line, the ruler is after the mode
     * message and must be redrawn */
    if (redrawing()
# ifdef FEAT_WINDOWS
	    && lastwin->w_status_height == 0
# endif
       )
	win_redr_ruler(lastwin, TRUE);
#endif
    redraw_cmdline = FALSE;
    clear_cmdline = FALSE;

    return length;
}

/*
 * Position for a mode message.
 */
    static void
msg_pos_mode(void)
{
    msg_col = 0;
    msg_row = Rows - 1;
}

/*
 * Delete mode message.  Used when ESC is typed which is expected to end
 * Insert mode (but Insert mode didn't end yet!).
 * Caller should check "mode_displayed".
 */
    void
unshowmode(int force)
{
    /*
     * Don't delete it right now, when not redrawing or inside a mapping.
     */
    if (!redrawing() || (!force && char_avail() && !KeyTyped))
	redraw_cmdline = TRUE;		/* delete mode later */
    else
    {
	msg_pos_mode();
	if (Recording)
	    recording_mode(hl_attr(HLF_CM));
	msg_clr_eos();
    }
}

    static void
recording_mode(int attr)
{
    MSG_PUTS_ATTR(_("recording"), attr);
    if (!shortmess(SHM_RECORDING))
    {
	char_u s[4];
	sprintf((char *)s, " @%c", Recording);
	MSG_PUTS_ATTR(s, attr);
    }
}

#if defined(FEAT_WINDOWS)
/*
 * Draw the tab pages line at the top of the Vim window.
 */
    static void
draw_tabline(void)
{
    int		tabcount = 0;
    tabpage_T	*tp;
    int		tabwidth;
    int		col = 0;
    int		scol = 0;
    int		attr;
    win_T	*wp;
    win_T	*cwp;
    int		wincount;
    int		modified;
    int		c;
    int		len;
    int		attr_sel = hl_attr(HLF_TPS);
    int		attr_nosel = hl_attr(HLF_TP);
    int		attr_fill = hl_attr(HLF_TPF);
    char_u	*p;
    int		room;
    int		use_sep_chars = (t_colors < 8
#ifdef FEAT_GUI
					    && !gui.in_use
#endif
					    );

    redraw_tabline = FALSE;

#ifdef FEAT_GUI_TABLINE
    /* Take care of a GUI tabline. */
    if (gui_use_tabline())
    {
	gui_update_tabline();
	return;
    }
#endif

    if (tabline_height() < 1)
	return;

#if defined(FEAT_STL_OPT)

    /* Init TabPageIdxs[] to zero: Clicking outside of tabs has no effect. */
    for (scol = 0; scol < Columns; ++scol)
	TabPageIdxs[scol] = 0;

    /* Use the 'tabline' option if it's set. */
    if (*p_tal != NUL)
    {
	int	save_called_emsg = called_emsg;

	/* Check for an error.  If there is one we would loop in redrawing the
	 * screen.  Avoid that by making 'tabline' empty. */
	called_emsg = FALSE;
	win_redr_custom(NULL, FALSE);
	if (called_emsg)
	    set_string_option_direct((char_u *)"tabline", -1,
					   (char_u *)"", OPT_FREE, SID_ERROR);
	called_emsg |= save_called_emsg;
    }
    else
#endif
    {
	for (tp = first_tabpage; tp != NULL; tp = tp->tp_next)
	    ++tabcount;

	tabwidth = (Columns - 1 + tabcount / 2) / tabcount;
	if (tabwidth < 6)
	    tabwidth = 6;

	attr = attr_nosel;
	tabcount = 0;
	scol = 0;
	for (tp = first_tabpage; tp != NULL && col < Columns - 4;
							     tp = tp->tp_next)
	{
	    scol = col;

	    if (tp->tp_topframe == topframe)
		attr = attr_sel;
	    if (use_sep_chars && col > 0)
		screen_putchar('|', 0, col++, attr);

	    if (tp->tp_topframe != topframe)
		attr = attr_nosel;

	    screen_putchar(' ', 0, col++, attr);

	    if (tp == curtab)
	    {
		cwp = curwin;
		wp = firstwin;
	    }
	    else
	    {
		cwp = tp->tp_curwin;
		wp = tp->tp_firstwin;
	    }

	    modified = FALSE;
	    for (wincount = 0; wp != NULL; wp = wp->w_next, ++wincount)
		if (bufIsChanged(wp->w_buffer))
		    modified = TRUE;
	    if (modified || wincount > 1)
	    {
		if (wincount > 1)
		{
		    vim_snprintf((char *)NameBuff, MAXPATHL, "%d", wincount);
		    len = (int)STRLEN(NameBuff);
		    if (col + len >= Columns - 3)
			break;
		    screen_puts_len(NameBuff, len, 0, col,
#if defined(FEAT_SYN_HL)
					 hl_combine_attr(attr, hl_attr(HLF_T))
#else
					 attr
#endif
					       );
		    col += len;
		}
		if (modified)
		    screen_puts_len((char_u *)"+", 1, 0, col++, attr);
		screen_putchar(' ', 0, col++, attr);
	    }

	    room = scol - col + tabwidth - 1;
	    if (room > 0)
	    {
		/* Get buffer name in NameBuff[] */
		get_trans_bufname(cwp->w_buffer);
		shorten_dir(NameBuff);
		len = vim_strsize(NameBuff);
		p = NameBuff;
#ifdef FEAT_MBYTE
		if (has_mbyte)
		    while (len > room)
		    {
			len -= ptr2cells(p);
			mb_ptr_adv(p);
		    }
		else
#endif
		    if (len > room)
		{
		    p += len - room;
		    len = room;
		}
		if (len > Columns - col - 1)
		    len = Columns - col - 1;

		screen_puts_len(p, (int)STRLEN(p), 0, col, attr);
		col += len;
	    }
	    screen_putchar(' ', 0, col++, attr);

	    /* Store the tab page number in TabPageIdxs[], so that
	     * jump_to_mouse() knows where each one is. */
	    ++tabcount;
	    while (scol < col)
		TabPageIdxs[scol++] = tabcount;
	}

	if (use_sep_chars)
	    c = '_';
	else
	    c = ' ';
	screen_fill(0, 1, col, (int)Columns, c, c, attr_fill);

	/* Put an "X" for closing the current tab if there are several. */
	if (first_tabpage->tp_next != NULL)
	{
	    screen_putchar('X', 0, (int)Columns - 1, attr_nosel);
	    TabPageIdxs[Columns - 1] = -999;
	}
    }

    /* Reset the flag here again, in case evaluating 'tabline' causes it to be
     * set. */
    redraw_tabline = FALSE;
}

/*
 * Get buffer name for "buf" into NameBuff[].
 * Takes care of special buffer names and translates special characters.
 */
    void
get_trans_bufname(buf_T *buf)
{
    if (buf_spname(buf) != NULL)
	vim_strncpy(NameBuff, buf_spname(buf), MAXPATHL - 1);
    else
	home_replace(buf, buf->b_fname, NameBuff, MAXPATHL, TRUE);
    trans_characters(NameBuff, MAXPATHL);
}
#endif

#if defined(FEAT_WINDOWS) || defined(FEAT_WILDMENU) || defined(FEAT_STL_OPT)
/*
 * Get the character to use in a status line.  Get its attributes in "*attr".
 */
    static int
fillchar_status(int *attr, int is_curwin)
{
    int fill;
    if (is_curwin)
    {
	*attr = hl_attr(HLF_S);
	fill = fill_stl;
    }
    else
    {
	*attr = hl_attr(HLF_SNC);
	fill = fill_stlnc;
    }
    /* Use fill when there is highlighting, and highlighting of current
     * window differs, or the fillchars differ, or this is not the
     * current window */
    if (*attr != 0 && ((hl_attr(HLF_S) != hl_attr(HLF_SNC)
			|| !is_curwin || firstwin == lastwin)
		    || (fill_stl != fill_stlnc)))
	return fill;
    if (is_curwin)
	return '^';
    return '=';
}
#endif

#ifdef FEAT_WINDOWS
/*
 * Get the character to use in a separator between vertically split windows.
 * Get its attributes in "*attr".
 */
    static int
fillchar_vsep(int *attr)
{
    *attr = hl_attr(HLF_C);
    if (*attr == 0 && fill_vert == ' ')
	return '|';
    else
	return fill_vert;
}
#endif

/*
 * Return TRUE if redrawing should currently be done.
 */
    int
redrawing(void)
{
    return (!RedrawingDisabled
		       && !(p_lz && char_avail() && !KeyTyped && !do_redraw));
}

/*
 * Return TRUE if printing messages should currently be done.
 */
    int
messaging(void)
{
    return (!(p_lz && char_avail() && !KeyTyped));
}

/*
 * Show current status info in ruler and various other places
 * If always is FALSE, only show ruler if position has changed.
 */
    void
showruler(int always)
{
    if (!always && !redrawing())
	return;
#ifdef FEAT_INS_EXPAND
    if (pum_visible())
    {
# ifdef FEAT_WINDOWS
	/* Don't redraw right now, do it later. */
	curwin->w_redr_status = TRUE;
# endif
	return;
    }
#endif
#if defined(FEAT_STL_OPT) && defined(FEAT_WINDOWS)
    if ((*p_stl != NUL || *curwin->w_p_stl != NUL) && curwin->w_status_height)
    {
	redraw_custom_statusline(curwin);
    }
    else
#endif
#ifdef FEAT_CMDL_INFO
	win_redr_ruler(curwin, always);
#endif

#ifdef FEAT_TITLE
    if (need_maketitle
# ifdef FEAT_STL_OPT
	    || (p_icon && (stl_syntax & STL_IN_ICON))
	    || (p_title && (stl_syntax & STL_IN_TITLE))
# endif
       )
	maketitle();
#endif
#ifdef FEAT_WINDOWS
    /* Redraw the tab pages line if needed. */
    if (redraw_tabline)
	draw_tabline();
#endif
}

#ifdef FEAT_CMDL_INFO
    static void
win_redr_ruler(win_T *wp, int always)
{
#define RULER_BUF_LEN 70
    char_u	buffer[RULER_BUF_LEN];
    int		row;
    int		fillchar;
    int		attr;
    int		empty_line = FALSE;
    colnr_T	virtcol;
    int		i;
    size_t	len;
    int		o;
#ifdef FEAT_WINDOWS
    int		this_ru_col;
    int		off = 0;
    int		width = Columns;
# define WITH_OFF(x) x
# define WITH_WIDTH(x) x
#else
# define WITH_OFF(x) 0
# define WITH_WIDTH(x) Columns
# define this_ru_col ru_col
#endif

    /* If 'ruler' off or redrawing disabled, don't do anything */
    if (!p_ru)
	return;

    /*
     * Check if cursor.lnum is valid, since win_redr_ruler() may be called
     * after deleting lines, before cursor.lnum is corrected.
     */
    if (wp->w_cursor.lnum > wp->w_buffer->b_ml.ml_line_count)
	return;

#ifdef FEAT_INS_EXPAND
    /* Don't draw the ruler while doing insert-completion, it might overwrite
     * the (long) mode message. */
# ifdef FEAT_WINDOWS
    if (wp == lastwin && lastwin->w_status_height == 0)
# endif
	if (edit_submode != NULL)
	    return;
    /* Don't draw the ruler when the popup menu is visible, it may overlap. */
    if (pum_visible())
	return;
#endif

#ifdef FEAT_STL_OPT
    if (*p_ruf)
    {
	int	save_called_emsg = called_emsg;

	called_emsg = FALSE;
	win_redr_custom(wp, TRUE);
	if (called_emsg)
	    set_string_option_direct((char_u *)"rulerformat", -1,
					   (char_u *)"", OPT_FREE, SID_ERROR);
	called_emsg |= save_called_emsg;
	return;
    }
#endif

    /*
     * Check if not in Insert mode and the line is empty (will show "0-1").
     */
    if (!(State & INSERT)
		&& *ml_get_buf(wp->w_buffer, wp->w_cursor.lnum, FALSE) == NUL)
	empty_line = TRUE;

    /*
     * Only draw the ruler when something changed.
     */
    validate_virtcol_win(wp);
    if (       redraw_cmdline
	    || always
	    || wp->w_cursor.lnum != wp->w_ru_cursor.lnum
	    || wp->w_cursor.col != wp->w_ru_cursor.col
	    || wp->w_virtcol != wp->w_ru_virtcol
#ifdef FEAT_VIRTUALEDIT
	    || wp->w_cursor.coladd != wp->w_ru_cursor.coladd
#endif
	    || wp->w_topline != wp->w_ru_topline
	    || wp->w_buffer->b_ml.ml_line_count != wp->w_ru_line_count
#ifdef FEAT_DIFF
	    || wp->w_topfill != wp->w_ru_topfill
#endif
	    || empty_line != wp->w_ru_empty)
    {
	cursor_off();
#ifdef FEAT_WINDOWS
	if (wp->w_status_height)
	{
	    row = W_WINROW(wp) + wp->w_height;
	    fillchar = fillchar_status(&attr, wp == curwin);
	    off = W_WINCOL(wp);
	    width = W_WIDTH(wp);
	}
	else
#endif
	{
	    row = Rows - 1;
	    fillchar = ' ';
	    attr = 0;
#ifdef FEAT_WINDOWS
	    width = Columns;
	    off = 0;
#endif
	}

	/* In list mode virtcol needs to be recomputed */
	virtcol = wp->w_virtcol;
	if (wp->w_p_list && lcs_tab1 == NUL)
	{
	    wp->w_p_list = FALSE;
	    getvvcol(wp, &wp->w_cursor, NULL, &virtcol, NULL);
	    wp->w_p_list = TRUE;
	}

	/*
	 * Some sprintfs return the length, some return a pointer.
	 * To avoid portability problems we use strlen() here.
	 */
	vim_snprintf((char *)buffer, RULER_BUF_LEN, "%ld,",
		(wp->w_buffer->b_ml.ml_flags & ML_EMPTY)
		    ? 0L
		    : (long)(wp->w_cursor.lnum));
	len = STRLEN(buffer);
	col_print(buffer + len, RULER_BUF_LEN - len,
			empty_line ? 0 : (int)wp->w_cursor.col + 1,
			(int)virtcol + 1);

	/*
	 * Add a "50%" if there is room for it.
	 * On the last line, don't print in the last column (scrolls the
	 * screen up on some terminals).
	 */
	i = (int)STRLEN(buffer);
	get_rel_pos(wp, buffer + i + 1, RULER_BUF_LEN - i - 1);
	o = i + vim_strsize(buffer + i + 1);
#ifdef FEAT_WINDOWS
	if (wp->w_status_height == 0)	/* can't use last char of screen */
#endif
	    ++o;
#ifdef FEAT_WINDOWS
	this_ru_col = ru_col - (Columns - width);
	if (this_ru_col < 0)
	    this_ru_col = 0;
#endif
	/* Never use more than half the window/screen width, leave the other
	 * half for the filename. */
	if (this_ru_col < (WITH_WIDTH(width) + 1) / 2)
	    this_ru_col = (WITH_WIDTH(width) + 1) / 2;
	if (this_ru_col + o < WITH_WIDTH(width))
	{
	    /* need at least 3 chars left for get_rel_pos() + NUL */
	    while (this_ru_col + o < WITH_WIDTH(width) && RULER_BUF_LEN > i + 4)
	    {
#ifdef FEAT_MBYTE
		if (has_mbyte)
		    i += (*mb_char2bytes)(fillchar, buffer + i);
		else
#endif
		    buffer[i++] = fillchar;
		++o;
	    }
	    get_rel_pos(wp, buffer + i, RULER_BUF_LEN - i);
	}
	/* Truncate at window boundary. */
#ifdef FEAT_MBYTE
	if (has_mbyte)
	{
	    o = 0;
	    for (i = 0; buffer[i] != NUL; i += (*mb_ptr2len)(buffer + i))
	    {
		o += (*mb_ptr2cells)(buffer + i);
		if (this_ru_col + o > WITH_WIDTH(width))
		{
		    buffer[i] = NUL;
		    break;
		}
	    }
	}
	else
#endif
	if (this_ru_col + (int)STRLEN(buffer) > WITH_WIDTH(width))
	    buffer[WITH_WIDTH(width) - this_ru_col] = NUL;

	screen_puts(buffer, row, this_ru_col + WITH_OFF(off), attr);
	i = redraw_cmdline;
	screen_fill(row, row + 1,
		this_ru_col + WITH_OFF(off) + (int)STRLEN(buffer),
		(int)(WITH_OFF(off) + WITH_WIDTH(width)),
		fillchar, fillchar, attr);
	/* don't redraw the cmdline because of showing the ruler */
	redraw_cmdline = i;
	wp->w_ru_cursor = wp->w_cursor;
	wp->w_ru_virtcol = wp->w_virtcol;
	wp->w_ru_empty = empty_line;
	wp->w_ru_topline = wp->w_topline;
	wp->w_ru_line_count = wp->w_buffer->b_ml.ml_line_count;
#ifdef FEAT_DIFF
	wp->w_ru_topfill = wp->w_topfill;
#endif
    }
}
#endif

#if defined(FEAT_LINEBREAK) || defined(PROTO)
/*
 * Return the width of the 'number' and 'relativenumber' column.
 * Caller may need to check if 'number' or 'relativenumber' is set.
 * Otherwise it depends on 'numberwidth' and the line count.
 */
    int
number_width(win_T *wp)
{
    int		n;
    linenr_T	lnum;

    if (wp->w_p_rnu && !wp->w_p_nu)
	/* cursor line shows "0" */
	lnum = wp->w_height;
    else
	/* cursor line shows absolute line number */
	lnum = wp->w_buffer->b_ml.ml_line_count;

    if (lnum == wp->w_nrwidth_line_count && wp->w_nuw_cached == wp->w_p_nuw)
	return wp->w_nrwidth_width;
    wp->w_nrwidth_line_count = lnum;

    n = 0;
    do
    {
	lnum /= 10;
	++n;
    } while (lnum > 0);

    /* 'numberwidth' gives the minimal width plus one */
    if (n < wp->w_p_nuw - 1)
	n = wp->w_p_nuw - 1;

    wp->w_nrwidth_width = n;
    wp->w_nuw_cached = wp->w_p_nuw;
    return n;
}
#endif

/*
 * Return the current cursor column. This is the actual position on the
 * screen. First column is 0.
 */
    int
screen_screencol(void)
{
    return screen_cur_col;
}

/*
 * Return the current cursor row. This is the actual position on the screen.
 * First row is 0.
 */
    int
screen_screenrow(void)
{
    return screen_cur_row;
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 2523,5;2523,12

// 3729,3;3729,10

// 6876,5;6876,12

// 7822,5;7822,12

// 7896,5;7896,12

// 10202,1;10202,8

