}
static vlm_media_sys_t *vlm_ControlMediaGetByName( vlm_t *p_vlm, const char *psz_name )
{
    int i;
    for( i = 0; i < p_vlm->i_media; i++ )
    {
        if( !strcmp( p_vlm->media[i]->cfg.psz_name, psz_name ) )
            return p_vlm->media[i];
    }
    return NULL;
}
