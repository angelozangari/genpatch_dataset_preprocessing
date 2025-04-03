}
static int NavigationCallback( vlc_object_t *p_this, char const *psz_cmd,
                               vlc_value_t oldval, vlc_value_t newval,
                               void *p_data )
{
    input_thread_t *p_input = (input_thread_t*)p_this;
    vlc_value_t     val;
    VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval);
    /* Issue a title change */
    val.i_int = (intptr_t)p_data;
    input_ControlPush( p_input, INPUT_CONTROL_SET_TITLE, &val );
    var_Change( p_input, "title", VLC_VAR_SETVALUE, &val, NULL );
    /* And a chapter change */
    input_ControlPush( p_input, INPUT_CONTROL_SET_SEEKPOINT, &newval );
    var_Change( p_input, "chapter", VLC_VAR_SETVALUE, &newval, NULL );
    return VLC_SUCCESS;
}
