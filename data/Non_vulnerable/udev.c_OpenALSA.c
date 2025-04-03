}
int OpenALSA (vlc_object_t *obj)
{
    static const struct subsys subsys = {
        "sound", alsa_get_mrl, alsa_get_name, ITEM_TYPE_CARD,
    };
    return Open (obj, &subsys);
}
