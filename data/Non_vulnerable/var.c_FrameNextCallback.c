}
static int FrameNextCallback( vlc_object_t *p_this, char const *psz_cmd,
                              vlc_value_t oldval, vlc_value_t newval,
                              void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    VLC_UNUSED(newval);
    input_ControlPush( p_input, INPUT_CONTROL_SET_FRAME_NEXT, NULL );
    return VLC_SUCCESS;
}
