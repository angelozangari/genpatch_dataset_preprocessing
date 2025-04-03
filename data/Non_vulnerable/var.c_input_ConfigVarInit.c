 *****************************************************************************/
void input_ConfigVarInit ( input_thread_t *p_input )
{
    /* Create Object Variables for private use only */
    if( !p_input->b_preparsing )
    {
        var_Create( p_input, "video", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "audio", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "spu", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "audio-track", VLC_VAR_INTEGER|VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-track", VLC_VAR_INTEGER|VLC_VAR_DOINHERIT );
        var_Create( p_input, "audio-language",
                    VLC_VAR_STRING|VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-language",
                    VLC_VAR_STRING|VLC_VAR_DOINHERIT );
        var_Create( p_input, "menu-language",
                    VLC_VAR_STRING|VLC_VAR_DOINHERIT );
        var_Create( p_input, "audio-track-id",
                    VLC_VAR_INTEGER|VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-track-id",
                    VLC_VAR_INTEGER|VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-file", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-autodetect-file", VLC_VAR_BOOL |
                    VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-autodetect-path", VLC_VAR_STRING |
                    VLC_VAR_DOINHERIT );
        var_Create( p_input, "sub-autodetect-fuzzy", VLC_VAR_INTEGER |
                    VLC_VAR_DOINHERIT );
        var_Create( p_input, "sout", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
        var_Create( p_input, "sout-all",   VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "sout-audio", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "sout-video", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "sout-spu", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "sout-keep",  VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
        var_Create( p_input, "input-repeat",
                    VLC_VAR_INTEGER|VLC_VAR_DOINHERIT );
        var_Create( p_input, "start-time", VLC_VAR_FLOAT|VLC_VAR_DOINHERIT );
        var_Create( p_input, "stop-time", VLC_VAR_FLOAT|VLC_VAR_DOINHERIT );
        var_Create( p_input, "run-time", VLC_VAR_FLOAT|VLC_VAR_DOINHERIT );
        var_Create( p_input, "input-fast-seek", VLC_VAR_BOOL|VLC_VAR_DOINHERIT );
        var_Create( p_input, "input-slave",
                    VLC_VAR_STRING | VLC_VAR_DOINHERIT );
        var_Create( p_input, "audio-desync",
                    VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
        var_Create( p_input, "cr-average",
                    VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
        var_Create( p_input, "clock-synchro",
                    VLC_VAR_INTEGER | VLC_VAR_DOINHERIT);
    }
    var_Create( p_input, "can-seek", VLC_VAR_BOOL );
    var_SetBool( p_input, "can-seek", true ); /* Fixed later*/
    var_Create( p_input, "can-pause", VLC_VAR_BOOL );
    var_SetBool( p_input, "can-pause", true ); /* Fixed later*/
    var_Create( p_input, "can-rate", VLC_VAR_BOOL );
    var_SetBool( p_input, "can-rate", false );
    var_Create( p_input, "can-rewind", VLC_VAR_BOOL );
    var_SetBool( p_input, "can-rewind", false );
    var_Create( p_input, "can-record", VLC_VAR_BOOL );
    var_SetBool( p_input, "can-record", false ); /* Fixed later*/
    var_Create( p_input, "record", VLC_VAR_BOOL );
    var_SetBool( p_input, "record", false );
    var_Create( p_input, "teletext-es", VLC_VAR_INTEGER );
    var_SetInteger( p_input, "teletext-es", -1 );
    var_Create( p_input, "signal-quality", VLC_VAR_FLOAT );
    var_SetFloat( p_input, "signal-quality", -1 );
    var_Create( p_input, "signal-strength", VLC_VAR_FLOAT );
    var_SetFloat( p_input, "signal-strength", -1 );
    var_Create( p_input, "program-scrambled", VLC_VAR_BOOL );
    var_SetBool( p_input, "program-scrambled", false );
    var_Create( p_input, "cache", VLC_VAR_FLOAT );
    var_SetFloat( p_input, "cache", 0.0 );
    /* */
    var_Create( p_input, "input-record-native", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    /* */
    var_Create( p_input, "access", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "demux", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "stream-filter", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    /* Meta */
    var_Create( p_input, "meta-title", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "meta-author", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "meta-artist", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "meta-genre", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "meta-copyright", VLC_VAR_STRING | VLC_VAR_DOINHERIT);
    var_Create( p_input, "meta-description", VLC_VAR_STRING|VLC_VAR_DOINHERIT);
    var_Create( p_input, "meta-date", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    var_Create( p_input, "meta-url", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
}
