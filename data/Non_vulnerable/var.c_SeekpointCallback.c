}
static int SeekpointCallback( vlc_object_t *p_this, char const *psz_cmd,
                              vlc_value_t oldval, vlc_value_t newval,
                              void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    vlc_value_t val, count;
    VLC_UNUSED(oldval); VLC_UNUSED(p_data);
    if( !strcmp( psz_cmd, "next-chapter" ) )
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_SEEKPOINT_NEXT, NULL );
        val.i_int = var_GetInteger( p_input, "chapter" ) + 1;
        var_Change( p_input, "chapter", VLC_VAR_CHOICESCOUNT, &count, NULL );
        if( val.i_int < count.i_int )
            var_Change( p_input, "chapter", VLC_VAR_SETVALUE, &val, NULL );
    }
    else if( !strcmp( psz_cmd, "prev-chapter" ) )
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_SEEKPOINT_PREV, NULL );
        val.i_int = var_GetInteger( p_input, "chapter" ) - 1;
        if( val.i_int >= 0 )
            var_Change( p_input, "chapter", VLC_VAR_SETVALUE, &val, NULL );
    }
    else
    {
        input_ControlPush( p_input, INPUT_CONTROL_SET_SEEKPOINT, &newval );
    }
    return VLC_SUCCESS;
}
