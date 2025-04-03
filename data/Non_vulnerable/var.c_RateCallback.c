}
static int RateCallback( vlc_object_t *p_this, char const *psz_cmd,
                         vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data); VLC_UNUSED(psz_cmd);
    newval.i_int = INPUT_RATE_DEFAULT / newval.f_float;
    input_ControlPush( p_input, INPUT_CONTROL_SET_RATE, &newval );
    return VLC_SUCCESS;
}
