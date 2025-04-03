 *****************************************************************************/
void input_ControlVarInit ( input_thread_t *p_input )
{
    vlc_value_t val, text;
    /* State */
    var_Create( p_input, "state", VLC_VAR_INTEGER );
    val.i_int = p_input->p->i_state;
    var_Change( p_input, "state", VLC_VAR_SETVALUE, &val, NULL );
    /* Rate */
    var_Create( p_input, "rate", VLC_VAR_FLOAT | VLC_VAR_DOINHERIT );
    var_Create( p_input, "frame-next", VLC_VAR_VOID );
    /* Position */
    var_Create( p_input, "position",  VLC_VAR_FLOAT );
    var_Create( p_input, "position-offset",  VLC_VAR_FLOAT );
    val.f_float = 0.0;
    var_Change( p_input, "position", VLC_VAR_SETVALUE, &val, NULL );
    /* Time */
    var_Create( p_input, "time",  VLC_VAR_TIME );
    var_Create( p_input, "time-offset",  VLC_VAR_TIME );    /* relative */
    val.i_time = 0;
    var_Change( p_input, "time", VLC_VAR_SETVALUE, &val, NULL );
    /* Bookmark */
    var_Create( p_input, "bookmark", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE |
                VLC_VAR_ISCOMMAND );
    val.psz_string = _("Bookmark");
    var_Change( p_input, "bookmark", VLC_VAR_SETTEXT, &val, NULL );
    /* Program */
    var_Create( p_input, "program", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE |
                VLC_VAR_DOINHERIT );
    var_Get( p_input, "program", &val );
    if( val.i_int <= 0 )
        var_Change( p_input, "program", VLC_VAR_DELCHOICE, &val, NULL );
    text.psz_string = _("Program");
    var_Change( p_input, "program", VLC_VAR_SETTEXT, &text, NULL );
    /* Programs */
    var_Create( p_input, "programs", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    text.psz_string = _("Programs");
    var_Change( p_input, "programs", VLC_VAR_SETTEXT, &text, NULL );
    /* Title */
    var_Create( p_input, "title", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE );
    text.psz_string = _("Title");
    var_Change( p_input, "title", VLC_VAR_SETTEXT, &text, NULL );
    /* Chapter */
    var_Create( p_input, "chapter", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE );
    text.psz_string = _("Chapter");
    var_Change( p_input, "chapter", VLC_VAR_SETTEXT, &text, NULL );
    /* Navigation The callback is added after */
    var_Create( p_input, "navigation", VLC_VAR_VARIABLE | VLC_VAR_HASCHOICE );
    text.psz_string = _("Navigation");
    var_Change( p_input, "navigation", VLC_VAR_SETTEXT, &text, NULL );
    /* Delay */
    var_Create( p_input, "audio-delay", VLC_VAR_TIME );
    val.i_time = INT64_C(1000) * var_GetInteger( p_input, "audio-desync" );
    var_Change( p_input, "audio-delay", VLC_VAR_SETVALUE, &val, NULL );
    var_Create( p_input, "spu-delay", VLC_VAR_TIME );
    val.i_time = 0;
    var_Change( p_input, "spu-delay", VLC_VAR_SETVALUE, &val, NULL );
    /* Video ES */
    var_Create( p_input, "video-es", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE );
    text.psz_string = _("Video Track");
    var_Change( p_input, "video-es", VLC_VAR_SETTEXT, &text, NULL );
    /* Audio ES */
    var_Create( p_input, "audio-es", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE );
    text.psz_string = _("Audio Track");
    var_Change( p_input, "audio-es", VLC_VAR_SETTEXT, &text, NULL );
    /* Spu ES */
    var_Create( p_input, "spu-es", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE );
    text.psz_string = _("Subtitle Track");
    var_Change( p_input, "spu-es", VLC_VAR_SETTEXT, &text, NULL );
    var_Create( p_input, "spu-choice", VLC_VAR_INTEGER );
    val.i_int = -1;
    var_Change( p_input, "spu-choice", VLC_VAR_SETVALUE, &val, NULL );
    /* Special read only objects variables for intf */
    var_Create( p_input, "bookmarks", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "length",  VLC_VAR_TIME );
    val.i_time = 0;
    var_Change( p_input, "length", VLC_VAR_SETVALUE, &val, NULL );
    var_Create( p_input, "bit-rate", VLC_VAR_INTEGER );
    var_Create( p_input, "sample-rate", VLC_VAR_INTEGER );
    if( !p_input->b_preparsing )
    {
        /* Special "intf-event" variable. */
        var_Create( p_input, "intf-event", VLC_VAR_INTEGER );
    }
    /* Add all callbacks
     * XXX we put callback only in non preparsing mode. We need to create the variable
     * unless someone want to check all var_Get/var_Change return value ... */
    if( !p_input->b_preparsing )
        InputAddCallbacks( p_input, p_input_callbacks );
}
