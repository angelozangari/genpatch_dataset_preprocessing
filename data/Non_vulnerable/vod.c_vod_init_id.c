 * with the data that was already set up */
int vod_init_id(vod_media_t *p_media, const char *psz_session, int es_id,
                sout_stream_id_sys_t *sout_id, rtp_format_t *rtp_fmt,
                uint32_t *ssrc, uint16_t *seq_init)
{
    media_es_t *p_es;
    if (p_media->psz_mux != NULL)
    {
        assert(p_media->i_es == 1);
        p_es = p_media->es[0];
    }
    else
    {
        p_es = NULL;
        /* No locking needed, the ES table can't be modified now */
        for (int i = 0; i < p_media->i_es; i++)
        {
            if (p_media->es[i]->es_id == es_id)
            {
                p_es = p_media->es[i];
                break;
            }
        }
        if (p_es == NULL)
            return VLC_EGENERIC;
    }
    memcpy(rtp_fmt, &p_es->rtp_fmt, sizeof(*rtp_fmt));
    if (p_es->rtp_fmt.fmtp != NULL)
        rtp_fmt->fmtp = strdup(p_es->rtp_fmt.fmtp);
    return RtspTrackAttach(p_media->rtsp, psz_session, p_es->rtsp_id,
                           sout_id, ssrc, seq_init);
}
 * with the data that was already set up */
int vod_init_id(vod_media_t *p_media, const char *psz_session, int es_id,
                sout_stream_id_sys_t *sout_id, rtp_format_t *rtp_fmt,
                uint32_t *ssrc, uint16_t *seq_init)
{
    media_es_t *p_es;
    if (p_media->psz_mux != NULL)
    {
        assert(p_media->i_es == 1);
        p_es = p_media->es[0];
    }
