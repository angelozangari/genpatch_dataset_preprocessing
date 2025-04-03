 */
picture_t *vout_GetPicture(vout_thread_t *vout)
{
    /* Get lock */
    vlc_mutex_lock(&vout->p->picture_lock);
    picture_t *picture = picture_pool_Get(vout->p->decoder_pool);
    if (picture) {
        picture_Reset(picture);
        VideoFormatCopyCropAr(&picture->format, &vout->p->original);
    }
    vlc_mutex_unlock(&vout->p->picture_lock);
    return picture;
}
