}
int OpenDisc (vlc_object_t *obj)
{
    static const struct subsys subsys = {
        "block", disc_get_mrl, disc_get_name, ITEM_TYPE_DISC,
    };
    return Open (obj, &subsys);
}
