}
static int vlm_ControlMediaGetId( vlm_t *p_vlm, const char *psz_name, int64_t *p_id )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetByName( p_vlm, psz_name );
    if( !p_media )
        return VLC_EGENERIC;
    *p_id = p_media->cfg.id;
    return VLC_SUCCESS;
}
