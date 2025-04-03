/* */
static picture_t *VoutVideoFilterInteractiveNewPicture(filter_t *filter)
{
    vout_thread_t *vout = (vout_thread_t*)filter->p_owner;
    picture_t *picture = picture_pool_Get(vout->p->private_pool);
    if (picture) {
        picture_Reset(picture);
        VideoFormatCopyCropAr(&picture->format, &filter->fmt_out.video);
    }
    return picture;
}
