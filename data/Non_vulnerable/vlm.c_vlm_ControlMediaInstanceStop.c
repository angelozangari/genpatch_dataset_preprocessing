}
static int vlm_ControlMediaInstanceStop( vlm_t *p_vlm, int64_t id, const char *psz_id )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    vlm_media_instance_sys_t *p_instance;
    if( !p_media )
        return VLC_EGENERIC;
    p_instance = vlm_ControlMediaInstanceGetByName( p_media, psz_id );
    if( !p_instance )
        return VLC_EGENERIC;
    vlm_MediaInstanceDelete( p_vlm, id, p_instance, p_media );
    return VLC_SUCCESS;
}
