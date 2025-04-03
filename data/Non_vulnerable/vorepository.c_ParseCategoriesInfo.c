}
static int ParseCategoriesInfo( addons_finder_t *p_finder, stream_t *p_stream )
{
    int i_num_entries_created = 0;
    const char *p_node;
    const char *attr, *value;
    int i_current_node_type;
    addon_entry_t *p_entry = NULL;
    xml_reader_t *p_xml_reader = xml_ReaderCreate( p_finder, p_stream );
    if( !p_xml_reader ) return 0;
    if( xml_ReaderNextNode( p_xml_reader, &p_node ) != XML_READER_STARTELEM )
    {
        msg_Err( p_finder, "invalid xml file" );
        goto end;
    }
    if ( strcmp( p_node, "videolan") )
    {
        msg_Err( p_finder, "unsupported XML data format" );
        goto end;
    }
    while( (i_current_node_type = xml_ReaderNextNode( p_xml_reader, &p_node )) > 0 )
    {
        switch( i_current_node_type )
        {
        case XML_READER_STARTELEM:
        {
            if ( ! strcmp( p_node, "addon" ) )
            {
                if ( p_entry ) /* Unclosed tag */
                    addon_entry_Release( p_entry );
                p_entry = addon_entry_New();
                p_entry->psz_source_module = strdup( ADDONS_MODULE_SHORTCUT );
                p_entry->e_flags = ADDON_MANAGEABLE;
                p_entry->e_state = ADDON_NOTINSTALLED;
                while( (attr = xml_ReaderNextAttr( p_xml_reader, &value )) )
                {
                    if ( !strcmp( attr, "type" ) )
                    {
                        p_entry->e_type = ReadType( value );
                    }
                    else if ( !strcmp( attr, "id" ) )
                    {
                        addons_uuid_read( value, & p_entry->uuid );
                    }
                    else if ( !strcmp( attr, "downloads" ) )
                    {
                        p_entry->i_downloads = atoi( value );
                        if ( p_entry->i_downloads < 0 )
                            p_entry->i_downloads = 0;
                    }
                    else if ( !strcmp( attr, "score" ) )
                    {
                        p_entry->i_score = atoi( value );
                        if ( p_entry->i_score < 0 )
                            p_entry->i_score = 0;
                        else if ( p_entry->i_score > ADDON_MAX_SCORE )
                            p_entry->i_score = ADDON_MAX_SCORE;
                    }
                    else if ( !strcmp( attr, "version" ) )
                    {
                        p_entry->psz_version = strdup( value );
                    }
                }
                break;
            }
            if ( !p_entry ) break;
            BINDNODE("name", p_entry->psz_name, TYPE_STRING)
            BINDNODE("archive", p_entry->psz_archive_uri, TYPE_STRING)
            BINDNODE("summary", p_entry->psz_summary, TYPE_STRING)
            BINDNODE("description", p_entry->psz_description, TYPE_STRING)
            BINDNODE("image", p_entry->psz_image_data, TYPE_STRING)
            BINDNODE("creator", p_entry->psz_author, TYPE_STRING)
            BINDNODE("sourceurl", p_entry->psz_source_uri, TYPE_STRING)
            data_pointer.e_type = TYPE_NONE;
            break;
        }
        case XML_READER_TEXT:
            if ( data_pointer.e_type == TYPE_NONE || !p_entry ) break;
            if ( data_pointer.e_type == TYPE_STRING )
                *data_pointer.u_data.ppsz = strdup( p_node );
            else
            if ( data_pointer.e_type == TYPE_LONG )
                *data_pointer.u_data.pl = atol( p_node );
            else
            if ( data_pointer.e_type == TYPE_INTEGER )
                *data_pointer.u_data.pi = atoi( p_node );
            break;
        case XML_READER_ENDELEM:
            if ( !p_entry ) break;
            if ( ! strcmp( p_node, "addon" ) )
            {
                /* then append entry */
                ARRAY_APPEND( p_finder->entries, p_entry );
                p_entry = NULL;
                i_num_entries_created++;
            }
            data_pointer.e_type = TYPE_NONE;
            break;
        default:
            break;
        }
    }
end:
   if ( p_entry ) /* Unclosed tag */
       addon_entry_Release( p_entry );
   xml_ReaderDelete( p_xml_reader );
   return i_num_entries_created;
}
