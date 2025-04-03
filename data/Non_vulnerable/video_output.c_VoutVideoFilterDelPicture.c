}
static void VoutVideoFilterDelPicture(filter_t *filter, picture_t *picture)
{
    VLC_UNUSED(filter);
    picture_Release(picture);
}
