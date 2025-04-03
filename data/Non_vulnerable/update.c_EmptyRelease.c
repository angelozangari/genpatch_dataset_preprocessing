 */
static void EmptyRelease( update_t *p_update )
{
    p_update->release.i_major = 0;
    p_update->release.i_minor = 0;
    p_update->release.i_revision = 0;
    FREENULL( p_update->release.psz_url );
    FREENULL( p_update->release.psz_desc );
}
