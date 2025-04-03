 *****************************************************************************/
static void ImportMarks( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    FILE *marksfile = OpenRelativeFile( p_access, "marks" );
    if( !marksfile )
        return;
    FILE *indexfile = OpenRelativeFile( p_access, "index" );
    if( !indexfile )
    {
        fclose( marksfile );
        return;
    }
    /* get the length of this recording (index stores 8 bytes per frame) */
    struct stat st;
    if( fstat( fileno( indexfile ), &st ) )
    {
        fclose( marksfile );
        fclose( indexfile );
        return;
    }
    int64_t i_frame_count = st.st_size / 8;
    /* Put all cut marks in a "dummy" title */
    input_title_t *p_marks = vlc_input_title_New();
    if( !p_marks )
    {
        fclose( marksfile );
        fclose( indexfile );
        return;
    }
    p_marks->psz_name = strdup( _("VDR Cut Marks") );
    p_marks->i_length = i_frame_count * (int64_t)( CLOCK_FREQ / p_sys->fps );
    p_marks->i_size = p_sys->size;
    /* offset for chapter positions */
    int i_chapter_offset = p_sys->fps / 1000 *
        var_InheritInteger( p_access, "vdr-chapter-offset" );
    /* minimum chapter size in frames */
    int i_min_chapter_size = p_sys->fps * MIN_CHAPTER_SIZE;
    /* the last chapter started at this frame (init to 0 so
     * we skip useless chapters near the beginning as well) */
    int64_t i_prev_chapter = 0;
    /* parse lines of the form "0:00:00.00 foobar" */
    char *line = NULL;
    size_t line_len;
    while( ReadLine( &line, &line_len, marksfile ) )
    {
        int64_t i_frame = ParseFrameNumber( line, p_sys->fps );
        /* skip chapters which are near the end or too close to each other */
        if( i_frame - i_prev_chapter < i_min_chapter_size ||
            i_frame >= i_frame_count - i_min_chapter_size )
            continue;
        i_prev_chapter = i_frame;
        /* move chapters (simple workaround for inaccurate cut marks) */
        if( i_frame > -i_chapter_offset )
            i_frame += i_chapter_offset;
        else
            i_frame = 0;
        uint64_t i_offset;
        uint16_t i_file_number;
        if( !ReadIndexRecord( indexfile, p_sys->b_ts_format,
            i_frame, &i_offset, &i_file_number ) )
            continue;
        if( i_file_number < 1 || i_file_number > FILE_COUNT )
            continue;
        /* add file sizes to get the "global" offset */
        seekpoint_t *sp = vlc_seekpoint_New();
        if( !sp )
            continue;
        sp->i_time_offset = i_frame * (int64_t)( CLOCK_FREQ / p_sys->fps );
        sp->i_byte_offset = i_offset;
        for( int i = 0; i + 1 < i_file_number; ++i )
            sp->i_byte_offset += FILE_SIZE( i );
        sp->psz_name = strdup( line );
        TAB_APPEND( p_marks->i_seekpoint, p_marks->seekpoint, sp );
    }
    /* add a chapter at the beginning if missing */
    if( p_marks->i_seekpoint > 0 && p_marks->seekpoint[0]->i_byte_offset > 0 )
    {
        seekpoint_t *sp = vlc_seekpoint_New();
        if( sp )
        {
            sp->i_byte_offset = 0;
            sp->i_time_offset = 0;
            sp->psz_name = strdup( _("Start") );
            TAB_INSERT( p_marks->i_seekpoint, p_marks->seekpoint, sp, 0 );
        }
    }
    if( p_marks->i_seekpoint > 0 )
        p_sys->p_marks = p_marks;
    else
        vlc_input_title_Delete( p_marks );
    fclose( marksfile );
    fclose( indexfile );
}
