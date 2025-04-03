}
int OpenV4L (vlc_object_t *obj)
{
    static const struct subsys subsys = {
        "video4linux", v4l_get_mrl, v4l_get_name, ITEM_TYPE_CARD,
    };
    return Open (obj, &subsys);
}
