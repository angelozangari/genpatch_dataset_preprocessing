}
static void Close(vlc_object_t *p_this)
{
    addons_finder_t *p_finder = (addons_finder_t*) p_this;
    if ( p_finder->p_sys->psz_tempfile )
    {
        vlc_unlink( p_finder->p_sys->psz_tempfile );
        free( p_finder->p_sys );
    }
}
