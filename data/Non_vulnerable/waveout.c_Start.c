 *****************************************************************************/
static int Start( audio_output_t *p_aout, audio_sample_format_t *restrict fmt )
{
    p_aout->time_get = WaveOutTimeGet;
    p_aout->play = Play;
    p_aout->pause = WaveOutPause;
    p_aout->flush = WaveOutFlush;
    /* Default behaviour is to use software gain */
    p_aout->sys->b_soft = true;
    char *dev = var_GetNonEmptyString( p_aout, "waveout-audio-device");
    uint32_t devid = findDeviceID( dev );
    if(devid == WAVE_MAPPER && dev != NULL && stricmp(dev,"wavemapper"))
        msg_Warn( p_aout, "configured audio device '%s' not available, "
                          "using default instead", dev );
    free( dev );
    WAVEOUTCAPS waveoutcaps;
    if(waveOutGetDevCaps( devid, &waveoutcaps,
                          sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
    {
      /* log debug some infos about driver, to know who to blame
         if it doesn't work */
        msg_Dbg( p_aout, "Drivername: %ls", waveoutcaps.szPname);
        msg_Dbg( p_aout, "Driver Version: %d.%d",
                          (waveoutcaps.vDriverVersion>>8)&255,
                          waveoutcaps.vDriverVersion & 255);
        msg_Dbg( p_aout, "Manufacturer identifier: 0x%x", waveoutcaps.wMid );
        msg_Dbg( p_aout, "Product identifier: 0x%x", waveoutcaps.wPid );
    }
    /* Open the device */
    if( AOUT_FMT_SPDIF(fmt) && var_InheritBool (p_aout, "spdif") )
    {
        if( OpenWaveOut( p_aout, devid, VLC_CODEC_SPDIFL,
                         fmt->i_physical_channels,
                         aout_FormatNbChannels( fmt ), fmt->i_rate, false )
            == VLC_SUCCESS )
        {
            fmt->i_format = VLC_CODEC_SPDIFL;
            /* Calculate the frame size in bytes */
            fmt->i_bytes_per_frame = AOUT_SPDIF_SIZE;
            fmt->i_frame_length = A52_FRAME_NB;
            p_aout->sys->i_buffer_size = fmt->i_bytes_per_frame;
            p_aout->sys->b_spdif = true;
        }
        else
            msg_Err( p_aout,
                     "cannot open waveout audio device for spdif fallback to PCM" );
    }
    if( fmt->i_format != VLC_CODEC_SPDIFL )
    {
       /*
         check for configured audio device!
       */
       fmt->i_format = var_InheritBool( p_aout, "waveout-float32" )?
           VLC_CODEC_FL32: VLC_CODEC_S16N;
        int max_chan = var_InheritInteger( p_aout, "waveout-audio-channels");
        int i_channels = aout_FormatNbChannels(fmt);
        i_channels = ( i_channels < max_chan )? i_channels: max_chan;
        do
        {
            switch(i_channels)
            {
                case 9:
                    fmt->i_physical_channels = AOUT_CHANS_8_1;
                    break;
                case 8:
                    fmt->i_physical_channels = AOUT_CHANS_7_1;
                    break;
                case 7:
                    fmt->i_physical_channels = AOUT_CHANS_7_0;
                    break;
                case 6:
                    fmt->i_physical_channels = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
                        | AOUT_CHAN_CENTER | AOUT_CHAN_REARLEFT
                        | AOUT_CHAN_REARRIGHT | AOUT_CHAN_LFE;
                    break;
                case 5:
                    fmt->i_physical_channels = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
                        | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT
                        | AOUT_CHAN_LFE;
                    break;
                case 4:
                    fmt->i_physical_channels = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
                        | AOUT_CHAN_REARLEFT | AOUT_CHAN_REARRIGHT;
                    break;
                case 3:
                    fmt->i_physical_channels = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
                        | AOUT_CHAN_LFE;
                    break;
                case 2:
                    fmt->i_physical_channels = AOUT_CHANS_STEREO;
                    break;
                case 1:
                default:
                    fmt->i_physical_channels = AOUT_CHAN_CENTER;
            }
            msg_Dbg( p_aout, "Trying %d channels", i_channels );
        }
        while( ( OpenWaveOutPCM( p_aout, devid, &fmt->i_format,
                                 fmt->i_physical_channels, i_channels,
                                 fmt->i_rate, false ) != VLC_SUCCESS ) &&
               --i_channels );
        if( !i_channels )
        {
            msg_Err(p_aout, "Waveout couldn't find appropriate channel mapping");
            return VLC_EGENERIC;
        }
        /* Calculate the frame size in bytes */
        aout_FormatPrepare( fmt );
        p_aout->sys->i_buffer_size = FRAME_SIZE * fmt->i_bytes_per_frame;
        if( waveoutcaps.dwSupport & WAVECAPS_VOLUME )
        {
            aout_GainRequest( p_aout, 1.0f );
            p_aout->sys->b_soft = false;
        }
        WaveoutMuteSet( p_aout, p_aout->sys->b_mute );
        p_aout->sys->b_spdif = false;
    }
    p_aout->sys->i_rate = fmt->i_rate;
    waveOutReset( p_aout->sys->h_waveout );
    /* Allocate silence buffer */
    p_aout->sys->p_silence_buffer =
        malloc( p_aout->sys->i_buffer_size );
    if( p_aout->sys->p_silence_buffer == NULL )
    {
        msg_Err( p_aout, "Couldn't alloc silence buffer... aborting");
        return VLC_ENOMEM;
    }
    p_aout->sys->i_repeat_counter = 0;
    /* Zero the buffer. WinCE doesn't have calloc(). */
    memset( p_aout->sys->p_silence_buffer, 0,
            p_aout->sys->i_buffer_size );
    /* Now we need to setup our waveOut play notification structure */
    p_aout->sys->i_frames = 0;
    p_aout->sys->i_played_length = 0;
    p_aout->sys->p_free_list = NULL;
    return VLC_SUCCESS;
}
