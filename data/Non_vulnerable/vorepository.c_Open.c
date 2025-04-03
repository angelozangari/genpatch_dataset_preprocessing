}
static int Open(vlc_object_t *p_this)
{
    addons_finder_t *p_finder = (addons_finder_t*) p_this;
    p_finder->p_sys = (addons_finder_sys_t*) malloc(sizeof(addons_finder_sys_t));
    if ( !p_finder->p_sys )
        return VLC_ENOMEM;
    p_finder->p_sys->psz_tempfile = NULL;
    if ( p_finder->psz_uri &&
         strcmp( "repo://"ADDONS_MODULE_SHORTCUT, p_finder->psz_uri ) &&
         memcmp( "repo://", p_finder->psz_uri, 8 ) )
        return VLC_EGENERIC;
    p_finder->pf_find = Find;
    p_finder->pf_retrieve = Retrieve;
    return VLC_SUCCESS;
}
