};
static picture_t *Filter(filter_t *filter, picture_t *src)
{
    filter_sys_t *sys = filter->p_sys;
    picture_t *dst = filter_NewPicture(filter);
    if (!dst) {
        picture_Release(src);
        return NULL;
    }
    const vlc_chroma_description_t *chroma = sys->chroma;
    for (unsigned i = 0; i < chroma->plane_count; i++)
         (sys->plane[i])(&dst->p[i], &src->p[i]);
    picture_CopyProperties(dst, src);
    picture_Release(src);
    return dst;
}
