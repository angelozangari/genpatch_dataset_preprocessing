}
static int VoutVideoFilterInteractiveAllocationSetup(filter_t *filter, void *data)
{
    filter->pf_video_buffer_new = VoutVideoFilterInteractiveNewPicture;
    filter->pf_video_buffer_del = VoutVideoFilterDelPicture;
    filter->p_owner             = data; /* vout */
    return VLC_SUCCESS;
}
