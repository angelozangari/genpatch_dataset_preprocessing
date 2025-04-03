}
static int vlm_ControlMediaInstanceClear( vlm_t *p_vlm, int64_t id )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    if( !p_media )
        return VLC_EGENERIC;
    while( p_media->i_instance > 0 )
        vlm_ControlMediaInstanceStop( p_vlm, id, p_media->instance[0]->psz_name );
    return VLC_SUCCESS;
}
