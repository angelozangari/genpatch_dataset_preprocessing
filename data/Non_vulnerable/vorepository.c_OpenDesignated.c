}
static int OpenDesignated(vlc_object_t *p_this)
{
    addons_finder_t *p_finder = (addons_finder_t*) p_this;
    if ( !p_finder->psz_uri
         || strncmp( "file://", p_finder->psz_uri, 7 )
         || strncmp( ".vlp", p_finder->psz_uri + strlen( p_finder->psz_uri ) - 4, 4 )
       )
        return VLC_EGENERIC;
    p_finder->pf_find = FindDesignated;
    p_finder->pf_retrieve = Retrieve;
    return VLC_SUCCESS;
}
