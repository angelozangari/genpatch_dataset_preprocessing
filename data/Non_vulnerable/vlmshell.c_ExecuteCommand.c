}
int ExecuteCommand( vlm_t *p_vlm, const char *psz_command,
                           vlm_message_t **pp_message )
{
    size_t i_command = 0;
    size_t i_command_len = strlen( psz_command );
    char *buf = malloc( i_command_len + 1 ), *psz_buf = buf;
    size_t i_ppsz_command_len = (3 + (i_command_len + 1) / 2);
    char **ppsz_command = malloc( i_ppsz_command_len * sizeof(char *) );
    vlm_message_t *p_message = NULL;
    int i_ret = 0;
    if( !psz_buf || !ppsz_command )
    {
        p_message = vlm_MessageNew( "Memory error",
                        "allocation failed for command of length %zu",
                        i_command_len );
        goto error;
    }
    /* First, parse the line and cut it */
    while( *psz_command != '\0' )
    {
        const char *psz_temp;
        if(isspace ((unsigned char)*psz_command))
        {
            psz_command++;
            continue;
        }
        /* support for comments */
        if( i_command == 0 && *psz_command == '#')
        {
            p_message = vlm_MessageSimpleNew( "" );
            goto success;
        }
        psz_temp = FindCommandEnd( psz_command );
        if( psz_temp == NULL )
        {
            p_message = vlm_MessageNew( "Incomplete command", "%s", psz_command );
            goto error;
        }
        assert (i_command < i_ppsz_command_len);
        ppsz_command[i_command] = psz_buf;
        memcpy (psz_buf, psz_command, psz_temp - psz_command);
        psz_buf[psz_temp - psz_command] = '\0';
        Unescape (psz_buf, psz_buf);
        i_command++;
        psz_buf += psz_temp - psz_command + 1;
        psz_command = psz_temp;
        assert (buf + i_command_len + 1 >= psz_buf);
    }
    /*
     * And then Interpret it
     */
#define IF_EXECUTE( name, check, cmd ) if( !strcmp(ppsz_command[0], name ) ) { if( (check) ) goto syntax_error;  if( (cmd) ) goto error; goto success; }
    if( i_command == 0 )
    {
        p_message = vlm_MessageSimpleNew( "" );
        goto success;
    }
    else IF_EXECUTE( "del",     (i_command != 2),   ExecuteDel(p_vlm, ppsz_command[1], &p_message) )
    else IF_EXECUTE( "show",    (i_command > 2),    ExecuteShow(p_vlm, i_command > 1 ? ppsz_command[1] : NULL, &p_message) )
    else IF_EXECUTE( "help",    (i_command != 1),   ExecuteHelp( &p_message ) )
    else IF_EXECUTE( "control", (i_command < 3),    ExecuteControl(p_vlm, ppsz_command[1], i_command - 2, &ppsz_command[2], &p_message) )
    else IF_EXECUTE( "save",    (i_command != 2),   ExecuteSave(p_vlm, ppsz_command[1], &p_message) )
    else IF_EXECUTE( "export",  (i_command != 1),   ExecuteExport(p_vlm, &p_message) )
    else IF_EXECUTE( "load",    (i_command != 2),   ExecuteLoad(p_vlm, ppsz_command[1], &p_message) )
    else IF_EXECUTE( "new",     (i_command < 3),    ExecuteNew(p_vlm, ppsz_command[1], ppsz_command[2], i_command-3, &ppsz_command[3], &p_message) )
    else IF_EXECUTE( "setup",   (i_command < 2),    ExecuteSetup(p_vlm, ppsz_command[1], i_command-2, &ppsz_command[2], &p_message) )
    else
    {
        p_message = vlm_MessageNew( ppsz_command[0], "Unknown VLM command" );
        goto error;
    }
#undef IF_EXECUTE
success:
    *pp_message = p_message;
    free( buf );
    free( ppsz_command );
    return VLC_SUCCESS;
syntax_error:
    i_ret = ExecuteSyntaxError( ppsz_command[0], pp_message );
    free( buf );
    free( ppsz_command );
    return i_ret;
error:
    *pp_message = p_message;
    free( buf );
    free( ppsz_command );
    return VLC_EGENERIC;
}
