*/
static uint32_t findDeviceID(char *psz_device_name)
{
    if( !psz_device_name )
       return WAVE_MAPPER;
    uint32_t wave_devices = waveOutGetNumDevs();
    for( uint32_t i = 0; i < wave_devices; i++ )
    {
        WAVEOUTCAPS caps;
        wchar_t dev_name[MAXPNAMELEN+32];
        if( waveOutGetDevCaps( i, &caps, sizeof(WAVEOUTCAPS) )
                                                          != MMSYSERR_NOERROR )
            continue;
        _snwprintf( dev_name, MAXPNAMELEN + 32, device_name_fmt,
                  caps.szPname, caps.wMid, caps.wPid );
        char *u8 = FromWide(dev_name);
        if( !stricmp(u8, psz_device_name) )
        {
            free( u8 );
            return i;
        }
        free( u8 );
    }
    return WAVE_MAPPER;
}
