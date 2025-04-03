}
static int EsDelayCallback ( vlc_object_t *p_this, char const *psz_cmd,
                             vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    if( !strcmp( psz_cmd, "audio-delay" ) )
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_AUDIO_DELAY, &newval );
    }
    else if( !strcmp( psz_cmd, "spu-delay" ) )
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_SPU_DELAY, &newval );
    }
    return VLC_SUCCESS;
}
