}
static int ESCallback( vlc_object_t *p_this, char const *psz_cmd,
                       vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    if( newval.i_int < 0 )
    {
        vlc_value_t v;
        /* Hack */
        if( !strcmp( psz_cmd, "audio-es" ) )
            v.i_int = -AUDIO_ES;
        else if( !strcmp( psz_cmd, "video-es" ) )
            v.i_int = -VIDEO_ES;
        else if( !strcmp( psz_cmd, "spu-es" ) )
            v.i_int = -SPU_ES;
        else
            v.i_int = 0;
        if( v.i_int != 0 )
            input_ControlPush( p_input, INPUT_CONTROL_SET_ES, &v );
    }
    else
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_ES, &newval );
    }
    return VLC_SUCCESS;
}
