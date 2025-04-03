}
static int TimeCallback( vlc_object_t *p_this, char const *psz_cmd,
                         vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    if( !strcmp( psz_cmd, "time-offset" ) )
    {
        mtime_t i_time = var_GetTime( p_input, "time" ) + newval.i_time;
        if( i_time < 0 )
            i_time = 0;
        var_SetTime( p_this, "time", i_time );
    }
    else
    {
        /* Update "position" for better intf behavour */
        const mtime_t i_length = var_GetTime( p_input, "length" );
        if( i_length > 0 && newval.i_time >= 0 && newval.i_time <= i_length )
        {
            vlc_value_t val;
            val.f_float = (double)newval.i_time/(double)i_length;
            var_Change( p_input, "position", VLC_VAR_SETVALUE, &val, NULL );
            /*
             * Notify the intf that a new event has been occurred.
             * XXX this is a bit hackish but it's the only way to do it now.
             */
            var_SetInteger( p_input, "intf-event", INPUT_EVENT_POSITION );
        }
        /* */
        input_ControlPush( p_input, INPUT_CONTROL_SET_TIME, &newval );
    }
    return VLC_SUCCESS;
}
