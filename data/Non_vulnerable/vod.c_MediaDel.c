}
static void MediaDel( vod_t *p_vod, vod_media_t *p_media )
{
    (void) p_vod;
    if (p_media->rtsp != NULL)
    {
        for (int i = 0; i < p_media->i_es; i++)
        {
            media_es_t *p_es = p_media->es[i];
            if (p_es->rtsp_id != NULL)
                RtspDelId(p_media->rtsp, p_es->rtsp_id);
        }
        RtspUnsetup(p_media->rtsp);
    }
    for( int i = 0; i < p_media->i_es; i++ )
    {
        free( p_media->es[i]->rtp_fmt.fmtp );
        free( p_media->es[i] );
    }
    free( p_media->es );
    free( p_media );
}
