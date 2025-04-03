 *****************************************************************************/
void vout_EnableFilter( vout_thread_t *p_vout, const char *psz_name,
                        bool b_add, bool b_setconfig )
{
    char *psz_parser;
    char *psz_string;
    const char *psz_filter_type;
    module_t *p_obj = module_find( psz_name );
    if( !p_obj )
    {
        msg_Err( p_vout, "Unable to find filter module \"%s\".", psz_name );
        return;
    }
    if( module_provides( p_obj, "video filter2" ) )
    {
        psz_filter_type = "video-filter";
    }
    else if( module_provides( p_obj, "sub source" ) )
    {
        psz_filter_type = "sub-source";
    }
    else if( module_provides( p_obj, "sub filter" ) )
    {
        psz_filter_type = "sub-filter";
    }
    else
    {
        msg_Err( p_vout, "Unknown video filter type." );
        return;
    }
    psz_string = var_GetString( p_vout, psz_filter_type );
    /* Todo : Use some generic chain manipulation functions */
    if( !psz_string ) psz_string = strdup("");
    psz_parser = strstr( psz_string, psz_name );
    if( b_add )
    {
        if( !psz_parser )
        {
            psz_parser = psz_string;
            if( asprintf( &psz_string, (*psz_string) ? "%s:%s" : "%s%s",
                          psz_string, psz_name ) == -1 )
            {
                free( psz_parser );
                return;
            }
            free( psz_parser );
        }
        else
        {
            free( psz_string );
            return;
        }
    }
    else
    {
        if( psz_parser )
        {
            memmove( psz_parser, psz_parser + strlen(psz_name) +
                            (*(psz_parser + strlen(psz_name)) == ':' ? 1 : 0 ),
                            strlen(psz_parser + strlen(psz_name)) + 1 );
            /* Remove trailing : : */
            if( *(psz_string+strlen(psz_string ) -1 ) == ':' )
            {
                *(psz_string+strlen(psz_string ) -1 ) = '\0';
            }
         }
         else
         {
             free( psz_string );
             return;
         }
    }
    if( b_setconfig )
    {
        config_PutPsz( p_vout, psz_filter_type, psz_string );
    }
    var_SetString( p_vout, psz_filter_type, psz_string );
    free( psz_string );
}
