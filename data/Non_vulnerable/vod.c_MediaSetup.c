}
static void MediaSetup( vod_t *p_vod, vod_media_t *p_media,
                        const char *psz_name )
{
    vod_sys_t *p_sys = p_vod->p_sys;
    char *psz_path;
    if( asprintf( &psz_path, "%s%s", p_sys->psz_rtsp_path, psz_name ) < 0 )
        return;
    p_media->rtsp = RtspSetup(VLC_OBJECT(p_vod), p_media, psz_path);
    free( psz_path );
    if (p_media->rtsp == NULL)
        return;
    for (int i = 0; i < p_media->i_es; i++)
    {
        media_es_t *p_es = p_media->es[i];
        p_es->rtsp_id = RtspAddId(p_media->rtsp, NULL, 0,
                                  p_es->rtp_fmt.clock_rate, -1);
    }
}
