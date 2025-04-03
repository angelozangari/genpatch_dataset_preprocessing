}
bool update_NeedUpgrade( update_t *p_update )
{
    assert( p_update );
    static const int current[4] = {
        PACKAGE_VERSION_MAJOR,
        PACKAGE_VERSION_MINOR,
        PACKAGE_VERSION_REVISION,
        PACKAGE_VERSION_EXTRA
    };
    const int latest[4] = {
        p_update->release.i_major,
        p_update->release.i_minor,
        p_update->release.i_revision,
        p_update->release.i_extra
    };
    for (unsigned i = 0; i < sizeof latest / sizeof *latest; i++) {
        /* there is a new version available */
        if (latest[i] > current[i])
            return true;
        /* current version is more recent than the latest version ?! */
        if (latest[i] < current[i])
            return false;
    }
    /* current version is not a release, it's a -git or -rc version */
    if (*PACKAGE_VERSION_DEV)
        return true;
    /* current version is latest version */
    return false;
}
}
bool update_NeedUpgrade( update_t *p_update )
{
    (void)p_update;
    return false;
}
