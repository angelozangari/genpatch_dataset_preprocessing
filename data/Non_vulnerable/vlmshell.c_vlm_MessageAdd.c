/* Add a child */
vlm_message_t *vlm_MessageAdd( vlm_message_t *p_message,
                               vlm_message_t *p_child )
{
    if( p_message == NULL ) return NULL;
    if( p_child )
    {
        TAB_APPEND( p_message->i_child, p_message->child, p_child );
    }
    return p_child;
}
