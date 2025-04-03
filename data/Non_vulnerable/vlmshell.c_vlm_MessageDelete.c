}
void vlm_MessageDelete( vlm_message_t *p_message )
{
    free( p_message->psz_name );
    free( p_message->psz_value );
    while( p_message->i_child-- )
        vlm_MessageDelete( p_message->child[p_message->i_child] );
    free( p_message->child );
    free( p_message );
}
