}
static int Setup( vlc_va_t *va, void **pp_hw_ctx, vlc_fourcc_t *pi_chroma,
                  int i_width, int i_height )
{
    vlc_va_sys_t *sys = va->sys;
    if( sys->i_surface_width == i_width &&
        sys->i_surface_height == i_height )
    {
        *pp_hw_ctx = &sys->hw_ctx;
        *pi_chroma = sys->i_surface_chroma;
        return VLC_SUCCESS;
    }
    *pp_hw_ctx = NULL;
    *pi_chroma = 0;
    if( sys->i_surface_width || sys->i_surface_height )
        DestroySurfaces( sys );
    if( i_width > 0 && i_height > 0 )
        return CreateSurfaces( sys, pp_hw_ctx, pi_chroma, i_width, i_height );
    return VLC_EGENERIC;
}
