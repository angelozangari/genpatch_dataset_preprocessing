}
int vod_check_range(vod_media_t *p_media, const char *psz_session,
                    int64_t start, int64_t end)
{
    (void) psz_session;
    if (p_media->i_length > 0 && (start > p_media->i_length
                                  || end > p_media->i_length))
        return VLC_EGENERIC;
    return VLC_SUCCESS;
}
