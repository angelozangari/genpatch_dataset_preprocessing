}
static int PositionCallback( vlc_object_t *p_this, char const *psz_cmd,
                             vlc_value_t oldval, vlc_value_t newval,
                             void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    if( !strcmp( psz_cmd, "position-offset" ) )
    {
        float f_position = var_GetFloat( p_input, "position" ) + newval.f_float;
        if( f_position < 0.0 )
            f_position = 0.0;
        else if( f_position > 1.0 )
            f_position = 1.0;
        var_SetFloat( p_this, "position", f_position );
    }
    else
    {
        /* Update "length" for better intf behavour */
        const mtime_t i_length = var_GetTime( p_input, "length" );
        if( i_length > 0 && newval.f_float >= 0.0 && newval.f_float <= 1.0 )
        {
            vlc_value_t val;
            val.i_time = i_length * newval.f_float;
            var_Change( p_input, "time", VLC_VAR_SETVALUE, &val, NULL );
        }
        /* */
        input_ControlPush( p_input, INPUT_CONTROL_SET_POSITION, &newval );
    }
    return VLC_SUCCESS;
}
