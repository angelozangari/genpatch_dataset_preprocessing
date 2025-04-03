}
static void sub_del_buffer(filter_t *filter, subpicture_t *subpic)
{
    VLC_UNUSED(filter);
    subpicture_Delete(subpic);
}
