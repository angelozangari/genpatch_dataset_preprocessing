};
static int ParseManifest( addons_finder_t *p_finder, addon_entry_t *p_entry,
                          const char *psz_tempfileuri, stream_t *p_stream )
{
    int i_num_entries_created = 0;
    const char *p_node;
    int i_current_node_type;
    /* attr */
    const char *attr, *value;
    /* temp reading */
    const char *psz_filename = NULL;
    int i_filetype = -1;
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
            BINDNODE("resource", psz_filename, TYPE_STRING)
            data_pointer.e_type = TYPE_NONE;
            /*
             * Manifests are not allowed to update addons properties
             * such as uuid, score, downloads, ...
             * On the other hand, repo API must not set files directly.
             */
            if ( ! strcmp( p_node, "resource" ) )
            {
                while( (attr = xml_ReaderNextAttr( p_xml_reader, &value )) )
                {
                    if ( !strcmp( attr, "type" ) )
                    {
                        i_filetype = ReadType( value );
                    }
                }
            }
            else if ( ! strcmp( p_node, "addon" ) )
            {
                while( (attr = xml_ReaderNextAttr( p_xml_reader, &value )) )
                {
                    if ( !strcmp( attr, "type" ) )
                    {
                        p_entry->e_type = ReadType( value );
                    }
                }
            }
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
            if ( ! strcmp( p_node, "resource" ) )
            {
                if ( psz_filename && i_filetype >= 0 )
                {
                    addon_file_t *p_file = malloc( sizeof(addon_file_t) );
                    p_file->e_filetype = i_filetype;
                    p_file->psz_filename = strdup( psz_filename );
                    if ( asprintf( & p_file->psz_download_uri, "%s!/%s",
                                   psz_tempfileuri, psz_filename  ) > 0 )
                    {
                        ARRAY_APPEND( p_entry->files, p_file );
                        msg_Dbg( p_finder, "manifest lists file %s extractable from %s",
                                 psz_filename, p_file->psz_download_uri );
                        i_num_entries_created++;
                    }
                    else
                    {
                        free( p_file->psz_filename );
                        free( p_file );
                    }
                }
                /* reset temp */
                psz_filename = NULL;
                i_filetype = -1;
            }
            data_pointer.e_type = TYPE_NONE;
            break;
        default:
            break;
        }
    }
end:
   xml_ReaderDelete( p_xml_reader );
   return i_num_entries_created;
}
