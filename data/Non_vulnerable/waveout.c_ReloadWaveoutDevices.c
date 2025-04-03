*/
static int ReloadWaveoutDevices( vlc_object_t *p_this, char const *psz_name,
                                 char ***values, char ***descs )
{
    int n = 0, nb_devices = waveOutGetNumDevs();
    VLC_UNUSED( p_this); VLC_UNUSED( psz_name );
    *values = xmalloc( (nb_devices + 1) * sizeof(char *) );
    *descs = xmalloc( (nb_devices + 1) * sizeof(char *) );
    (*values)[n] = strdup( "wavemapper" );
    (*descs)[n] = strdup( _("Microsoft Soundmapper") );
    n++;
    for(int i = 0; i < nb_devices; i++)
    {
        WAVEOUTCAPS caps;
        wchar_t dev_name[MAXPNAMELEN+32];
        if(waveOutGetDevCaps(i, &caps, sizeof(WAVEOUTCAPS))
                                                           != MMSYSERR_NOERROR)
            continue;
        _snwprintf(dev_name, MAXPNAMELEN + 32, device_name_fmt,
                   caps.szPname, caps.wMid, caps.wPid);
        (*values)[n] = FromWide( dev_name );
        (*descs)[n] = strdup( (*values)[n] );
        n++;
    }
    return n;
}
