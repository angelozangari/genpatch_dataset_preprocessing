}
static int VoutVideoFilterStaticAllocationSetup(filter_t *filter, void *data)
{
    filter->pf_video_buffer_new = VoutVideoFilterStaticNewPicture;
    filter->pf_video_buffer_del = VoutVideoFilterDelPicture;
    filter->p_owner             = data; /* vout */
    return VLC_SUCCESS;
}
