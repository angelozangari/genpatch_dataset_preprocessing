}
static int Get( vlc_va_t *external, void **opaque, uint8_t **data )
{
    VLC_UNUSED( external );
    *data = (uint8_t *)1; // dummy
    (void) opaque;
    return VLC_SUCCESS;
}
// Never called
static int Get( vlc_va_t *external, void **opaque, uint8_t **data )
{
    VLC_UNUSED( external );
    (void) data;
    (void) opaque;
    return VLC_SUCCESS;
}
