}
static int ParseVobSubIDX( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    text_t      *txt = &p_sys->txt;
    char        *line;
    vobsub_track_t *current_tk = NULL;
    for( ;; )
    {
        if( ( line = TextGetLine( txt ) ) == NULL )
        {
            return( VLC_EGENERIC );
        }
        if( *line == 0 || *line == '\r' || *line == '\n' || *line == '#' )
        {
            continue;
        }
        else if( !strncmp( "size:", line, 5 ) )
        {
            /* Store the original size of the video */
            if( vobsub_size_parse( line, &p_sys->i_original_frame_width,
                                   &p_sys->i_original_frame_height ) == VLC_SUCCESS )
            {
                msg_Dbg( p_demux, "original frame size: %dx%d", p_sys->i_original_frame_width, p_sys->i_original_frame_height );
            }
            else
            {
                msg_Warn( p_demux, "reading original frame size failed" );
            }
        }
        else if( !strncmp( "palette:", line, 8 ) )
        {
            if( vobsub_palette_parse( line, p_sys->palette ) == VLC_SUCCESS )
            {
                p_sys->b_palette = true;
                msg_Dbg( p_demux, "vobsub palette read" );
            }
            else
            {
                msg_Warn( p_demux, "reading original palette failed" );
            }
        }
        else if( !strncmp( "id:", line, 3 ) )
        {
            char language[33]; /* Usually 2 or 3 letters, sometimes more.
                                  Spec (or lack of) doesn't define any limit */
            int i_track_id;
            es_format_t fmt;
            /* Lets start a new track */
            if( sscanf( line, "id: %32[^ ,], index: %d",
                        language, &i_track_id ) != 2 )
            {
                if( sscanf( line, "id: , index: %d", &i_track_id ) != 1 )
                {
                    msg_Warn( p_demux, "reading new track failed" );
                    continue;
                }
                language[0] = '\0';
            }
            p_sys->i_tracks++;
            p_sys->track = xrealloc( p_sys->track,
                    sizeof( vobsub_track_t ) * (p_sys->i_tracks + 1 ) );
            /* Init the track */
            current_tk = &p_sys->track[p_sys->i_tracks - 1];
            memset( current_tk, 0, sizeof( vobsub_track_t ) );
            current_tk->i_current_subtitle = 0;
            current_tk->i_subtitles = 0;
            current_tk->p_subtitles = xmalloc( sizeof( subtitle_t ) );
            current_tk->i_track_id = i_track_id;
            current_tk->i_delay = (int64_t)0;
            es_format_Init( &fmt, SPU_ES, VLC_CODEC_SPU );
            fmt.subs.spu.i_original_frame_width = p_sys->i_original_frame_width;
            fmt.subs.spu.i_original_frame_height = p_sys->i_original_frame_height;
            fmt.psz_language = language;
            if( p_sys->b_palette )
            {
                fmt.subs.spu.palette[0] = 0xBeef;
                memcpy( &fmt.subs.spu.palette[1], p_sys->palette, 16 * sizeof( uint32_t ) );
            }
            current_tk->p_es = es_out_Add( p_demux->out, &fmt );
            msg_Dbg( p_demux, "new vobsub track detected" );
        }
        else if( !strncmp( line, "timestamp:", 10 ) )
        {
            /*
             * timestamp: [sign]hh:mm:ss:mss, filepos: loc
             * loc is the hex location of the spu in the .sub file
             */
            int h, m, s, ms, count, loc = 0;
            int i_sign = 1;
            int64_t i_start, i_location = 0;
            if( p_sys->i_tracks > 0 &&
                sscanf( line, "timestamp: %d%n:%d:%d:%d, filepos: %x",
                        &h, &count, &m, &s, &ms, &loc ) >= 5  )
            {
                vobsub_track_t *current_tk = &p_sys->track[p_sys->i_tracks - 1];
                subtitle_t *current_sub;
                if( line[count-3] == '-' )
                {
                    i_sign = -1;
                    h = -h;
                }
                i_start = (int64_t) ( h * 3600*1000 +
                            m * 60*1000 +
                            s * 1000 +
                            ms ) * 1000;
                i_location = loc;
                current_tk->i_subtitles++;
                current_tk->p_subtitles =
                    xrealloc( current_tk->p_subtitles,
                      sizeof( subtitle_t ) * (current_tk->i_subtitles + 1 ) );
                current_sub = &current_tk->p_subtitles[current_tk->i_subtitles - 1];
                current_sub->i_start = i_start * i_sign;
                current_sub->i_start += current_tk->i_delay;
                current_sub->i_vobsub_location = i_location;
            }
            else
            {
                msg_Warn( p_demux, "reading timestamp failed" );
            }
        }
        else if( !strncasecmp( line, "delay:", 6 ) )
        {
            /*
             * delay: [sign]hh:mm:ss:mss
             */
            int h, m, s, ms, count = 0;
            int i_sign = 1;
            int64_t i_gap = 0;
            if( p_sys->i_tracks > 0 &&
                sscanf( line, "%*celay: %d%n:%d:%d:%d",
                        &h, &count, &m, &s, &ms ) >= 4 )
            {
                vobsub_track_t *current_tk = &p_sys->track[p_sys->i_tracks - 1];
                if( line[count-3] == '-' )
                {
                    i_sign = -1;
                    h = -h;
                }
                i_gap = (int64_t) ( h * 3600*1000 +
                            m * 60*1000 +
                            s * 1000 +
                            ms ) * 1000;
                current_tk->i_delay = current_tk->i_delay + (i_gap * i_sign);
                msg_Dbg( p_demux, "sign: %+d gap: %+"PRId64" global delay: %+"PRId64"",
                         i_sign, i_gap, current_tk->i_delay );
            }
            else
            {
                msg_Warn( p_demux, "reading delay failed" );
            }
        }
    }
    return( 0 );
}
