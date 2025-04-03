}
static int vlm_ControlMediaInstanceSetTimePosition( vlm_t *p_vlm, int64_t id, const char *psz_id, int64_t i_time, double d_position )
{
    vlm_media_sys_t *p_media = vlm_ControlMediaGetById( p_vlm, id );
    vlm_media_instance_sys_t *p_instance;
    if( !p_media )
        return VLC_EGENERIC;
    p_instance = vlm_ControlMediaInstanceGetByName( p_media, psz_id );
    if( !p_instance || !p_instance->p_input )
        return VLC_EGENERIC;
    if( i_time >= 0 )
        return var_SetTime( p_instance->p_input, "time", i_time );
    else if( d_position >= 0 && d_position <= 100 )
        return var_SetFloat( p_instance->p_input, "position", d_position );
    return VLC_EGENERIC;
}
