 *****************************************************************************/
vlm_message_t *vlm_MessageSimpleNew( const char *psz_name )
{
    if( !psz_name ) return NULL;
    vlm_message_t *p_message = malloc( sizeof(*p_message) );
    if( !p_message )
        return NULL;
    p_message->psz_name = strdup( psz_name );
    if( !p_message->psz_name )
    {
        free( p_message );
        return NULL;
    }
    p_message->psz_value = NULL;
    p_message->i_child = 0;
    p_message->child = NULL;
    return p_message;
}
