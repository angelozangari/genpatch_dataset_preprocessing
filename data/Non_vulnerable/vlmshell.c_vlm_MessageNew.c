}
vlm_message_t *vlm_MessageNew( const char *psz_name,
                               const char *psz_format, ... )
{
    vlm_message_t *p_message = vlm_MessageSimpleNew( psz_name );
    va_list args;
    if( !p_message )
        return NULL;
    assert( psz_format );
    va_start( args, psz_format );
    if( vasprintf( &p_message->psz_value, psz_format, args ) == -1 )
        p_message->psz_value = NULL;
    va_end( args );
    if( !p_message->psz_value )
    {
        vlm_MessageDelete( p_message );
        return NULL;
    }
    return p_message;
}
