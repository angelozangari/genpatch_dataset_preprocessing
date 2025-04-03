}
static void Close(vlc_object_t *object)
{
    filter_t     *filter = (filter_t *)object;
    filter_sys_t *sys    = filter->p_sys;
    free(sys);
}
