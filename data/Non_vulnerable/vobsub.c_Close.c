 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    if( p_sys->p_vobsub_stream )
        stream_Delete( p_sys->p_vobsub_stream );
    /* Clean all subs from all tracks */
    for( int i = 0; i < p_sys->i_tracks; i++ )
        free( p_sys->track[i].p_subtitles );
    free( p_sys->track );
    free( p_sys );
}
