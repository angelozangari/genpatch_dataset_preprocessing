/* */
int vout_GetSnapshot(vout_thread_t *vout,
                     block_t **image_dst, picture_t **picture_dst,
                     video_format_t *fmt,
                     const char *type, mtime_t timeout)
{
    picture_t *picture = vout_snapshot_Get(&vout->p->snapshot, timeout);
    if (!picture) {
        msg_Err(vout, "Failed to grab a snapshot");
        return VLC_EGENERIC;
    }
    if (image_dst) {
        vlc_fourcc_t codec = VLC_CODEC_PNG;
        if (type && image_Type2Fourcc(type))
            codec = image_Type2Fourcc(type);
        const int override_width  = var_InheritInteger(vout, "snapshot-width");
        const int override_height = var_InheritInteger(vout, "snapshot-height");
        if (picture_Export(VLC_OBJECT(vout), image_dst, fmt,
                           picture, codec, override_width, override_height)) {
            msg_Err(vout, "Failed to convert image for snapshot");
            picture_Release(picture);
            return VLC_EGENERIC;
        }
    }
    if (picture_dst)
        *picture_dst = picture;
    else
        picture_Release(picture);
    return VLC_SUCCESS;
}
/* */
int vout_GetSnapshot(vout_thread_t *vout,
                     block_t **image_dst, picture_t **picture_dst,
                     video_format_t *fmt,
                     const char *type, mtime_t timeout)
{
    picture_t *picture = vout_snapshot_Get(&vout->p->snapshot, timeout);
    if (!picture) {
        msg_Err(vout, "Failed to grab a snapshot");
        return VLC_EGENERIC;
    }
