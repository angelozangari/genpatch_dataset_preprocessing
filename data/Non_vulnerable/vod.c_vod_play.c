 * clean this up */
void vod_play(vod_media_t *p_media, const char *psz_session,
              int64_t *start, int64_t end)
{
    if (vod_check_range(p_media, psz_session, *start, end) != VLC_SUCCESS)
        return;
    /* We're passing the #vod{} sout chain here */
    vod_MediaControl(p_media->p_vod, p_media, psz_session,
                     VOD_MEDIA_PLAY, "vod", start);
}
