}
void vod_stop(vod_media_t *p_media, const char *psz_session)
{
    CommandPush(p_media->p_vod, RTSP_CMD_TYPE_STOP, p_media, psz_session);
}
