 *****************************************************************************/
static int Load( vlm_t *vlm, char *file )
{
    char *pf = file;
    int  i_line = 1;
    while( *pf != '\0' )
    {
        vlm_message_t *message = NULL;
        int i_end = 0;
        while( pf[i_end] != '\n' && pf[i_end] != '\0' && pf[i_end] != '\r' )
        {
            i_end++;
        }
        if( pf[i_end] == '\r' || pf[i_end] == '\n' )
        {
            pf[i_end] = '\0';
            i_end++;
            if( pf[i_end] == '\n' ) i_end++;
        }
        if( *pf && ExecuteCommand( vlm, pf, &message ) )
        {
            if( message )
            {
                if( message->psz_value )
                    msg_Err( vlm, "Load error on line %d: %s: %s",
                             i_line, message->psz_name, message->psz_value );
                vlm_MessageDelete( message );
            }
            return 1;
        }
        if( message ) vlm_MessageDelete( message );
        pf += i_end;
        i_line++;
    }
    return 0;
}
