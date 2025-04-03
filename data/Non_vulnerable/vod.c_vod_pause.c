}
void vod_pause(vod_media_t *p_media, const char *psz_session, int64_t *npt)
{
    vod_MediaControl(p_media->p_vod, p_media, psz_session,
                     VOD_MEDIA_PAUSE, npt);
}
