}
static void Close (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    SRC_STATE *s = (SRC_STATE *)filter->p_sys;
    src_delete (s);
}
