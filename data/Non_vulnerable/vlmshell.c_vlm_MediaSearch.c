 *****************************************************************************/
vlm_media_sys_t *vlm_MediaSearch( vlm_t *vlm, const char *psz_name )
{
    int i;
    for( i = 0; i < vlm->i_media; i++ )
    {
        if( strcmp( psz_name, vlm->media[i]->cfg.psz_name ) == 0 )
            return vlm->media[i];
    }
    return NULL;
}
