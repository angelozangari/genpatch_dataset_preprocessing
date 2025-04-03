}
static vlm_media_instance_sys_t *vlm_ControlMediaInstanceGetByName( vlm_media_sys_t *p_media, const char *psz_id )
{
    int i;
    for( i = 0; i < p_media->i_instance; i++ )
    {
        const char *psz = p_media->instance[i]->psz_name;
        if( ( psz == NULL && psz_id == NULL ) ||
            ( psz && psz_id && !strcmp( psz, psz_id ) ) )
            return p_media->instance[i];
    }
    return NULL;
}
