 *****************************************************************************/
static int Open ( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    char *psz_vobname, *s;
    int i_len;
    if( ( s = stream_ReadLine( p_demux->s ) ) != NULL )
    {
        if( !strcasestr( s, "# VobSub index file" ) )
        {
            msg_Dbg( p_demux, "this doesn't seem to be a vobsub file" );
            free( s );
            if( stream_Seek( p_demux->s, 0 ) )
            {
                msg_Warn( p_demux, "failed to rewind" );
            }
            return VLC_EGENERIC;
        }
        free( s );
    }
    else
    {
        msg_Dbg( p_demux, "could not read vobsub IDX file" );
        return VLC_EGENERIC;
    }
    /* */
    p_demux->p_sys = p_sys = malloc( sizeof( demux_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    p_sys->i_length = 0;
    p_sys->p_vobsub_stream = NULL;
    p_sys->i_tracks = 0;
    p_sys->track = malloc( sizeof( vobsub_track_t ) );
    if( unlikely( !p_sys->track ) )
        goto error;
    p_sys->i_original_frame_width = -1;
    p_sys->i_original_frame_height = -1;
    p_sys->b_palette = false;
    memset( p_sys->palette, 0, 16 * sizeof( uint32_t ) );
    /* Load the whole file */
    TextLoad( &p_sys->txt, p_demux->s );
    /* Parse it */
    ParseVobSubIDX( p_demux );
    /* Unload */
    TextUnload( &p_sys->txt );
    /* Find the total length of the vobsubs */
    if( p_sys->i_tracks > 0 )
    {
        for( int i = 0; i < p_sys->i_tracks; i++ )
        {
            if( p_sys->track[i].i_subtitles > 1 )
            {
                if( p_sys->track[i].p_subtitles[p_sys->track[i].i_subtitles-1].i_start > p_sys->i_length )
                    p_sys->i_length = (int64_t) p_sys->track[i].p_subtitles[p_sys->track[i].i_subtitles-1].i_start + ( 1 *1000 *1000 );
            }
        }
    }
    if( asprintf( &psz_vobname, "%s://%s", p_demux->psz_access, p_demux->psz_location ) == -1 )
        goto error;
    i_len = strlen( psz_vobname );
    if( i_len >= 4 ) memcpy( psz_vobname + i_len - 4, ".sub", 4 );
    /* open file */
    p_sys->p_vobsub_stream = stream_UrlNew( p_demux, psz_vobname );
    if( p_sys->p_vobsub_stream == NULL )
    {
        msg_Err( p_demux, "couldn't open .sub Vobsub file: %s",
                 psz_vobname );
        free( psz_vobname );
        goto error;
    }
    free( psz_vobname );
    p_demux->pf_demux = Demux;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
error:
    /* Clean all subs from all tracks */
    for( int i = 0; i < p_sys->i_tracks; i++ )
        free( p_sys->track[i].p_subtitles );
    free( p_sys->track );
    free( p_sys );
    return VLC_EGENERIC;
}
