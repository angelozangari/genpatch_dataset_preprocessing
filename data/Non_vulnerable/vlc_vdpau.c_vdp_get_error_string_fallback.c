}
static const char *vdp_get_error_string_fallback(const vdp_t *vdp,
                                                 VdpStatus status)
{
    (void) vdp;
    return (status != VDP_STATUS_OK) ? "Unknown error" : "No error";
}
