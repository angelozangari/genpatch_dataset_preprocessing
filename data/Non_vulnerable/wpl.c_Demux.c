}
static int Demux( demux_t *p_demux )
{
    char       *psz_line;
    input_item_t *p_current_input = GetCurrentItem(p_demux);
    input_item_node_t *p_subitems = input_item_node_Create( p_current_input );
    while( (psz_line = stream_ReadLine( p_demux->s )) )
    {
        char *psz_parse = psz_line;
        /* Skip leading tabs and spaces */
        while( *psz_parse == ' '  || *psz_parse == '\t' ||
               *psz_parse == '\n' || *psz_parse == '\r' )
            psz_parse++;
        /* if the line is the uri of the media item */
        if( !strncasecmp( psz_parse, "<media src=\"", strlen( "<media src=\"" ) ) )
        {
            char *psz_uri = psz_parse + strlen( "<media src=\"" );
            psz_parse = strchr( psz_uri, '"' );
            if( psz_parse != NULL )
            {
                input_item_t *p_input;
                *psz_parse = '\0';
                resolve_xml_special_chars( psz_uri );
                psz_uri = ProcessMRL( psz_uri, p_demux->p_sys->psz_prefix );
                p_input = input_item_NewExt( psz_uri, psz_uri,
                                        0, NULL, 0, -1 );
                input_item_node_AppendItem( p_subitems, p_input );
                vlc_gc_decref( p_input );
            }
        }
        /* Fetch another line */
        free( psz_line );
    }
    input_item_node_PostAndDelete( p_subitems );
    vlc_gc_decref(p_current_input);
    var_Destroy( p_demux, "wpl-extvlcopt" );
    return 0; /* Needed for correct operation of go back */
}
