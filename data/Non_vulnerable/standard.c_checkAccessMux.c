}
static void checkAccessMux( sout_stream_t *p_stream, char *psz_access,
                            char *psz_mux )
{
    if( !strncmp( psz_access, "mmsh", 4 ) && strncmp( psz_mux, "asfh", 4 ) )
        msg_Err( p_stream, "mmsh output is only valid with asfh mux" );
    else if( strncmp( psz_access, "file", 4 ) &&
            ( !strncmp( psz_mux, "mov", 3 ) || !strncmp( psz_mux, "mp4", 3 ) ) )
        msg_Err( p_stream, "mov and mp4 mux are only valid with file output" );
    else if( !strncmp( psz_access, "udp", 3 ) )
    {
        if( !strncmp( psz_mux, "ffmpeg", 6 ) || !strncmp( psz_mux, "avformat", 8 ) )
        {   /* why would you use ffmpeg's ts muxer ? YOU DON'T LOVE VLC ??? */
            char *psz_ffmpeg_mux = var_CreateGetString( p_stream, "sout-avformat-mux" );
            if( !psz_ffmpeg_mux || strncmp( psz_ffmpeg_mux, "mpegts", 6 ) )
                msg_Err( p_stream, "UDP output is only valid with TS mux" );
            free( psz_ffmpeg_mux );
        }
        else if( strncmp( psz_mux, "ts", 2 ) )
            msg_Err( p_stream, "UDP output is only valid with TS mux" );
    }
}
