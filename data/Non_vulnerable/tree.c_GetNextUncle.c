}
playlist_item_t *GetNextUncle( playlist_t *p_playlist, playlist_item_t *p_item,
                               playlist_item_t *p_root )
{
    playlist_item_t *p_parent = p_item->p_parent;
    playlist_item_t *p_grandparent;
    bool b_found = false;
    (void)p_playlist;
    if( p_parent != NULL )
    {
        p_grandparent = p_parent->p_parent;
        while( p_grandparent )
        {
            int i;
            for( i = 0 ; i< p_grandparent->i_children ; i++ )
            {
                if( p_parent == p_grandparent->pp_children[i] )
                {
                    PL_DEBUG2( "parent %s found as child %i of grandparent %s",
                               p_parent->p_input->psz_name, i,
                               p_grandparent->p_input->psz_name );
                    b_found = true;
                    break;
                }
            }
            if( b_found && i + 1 < p_grandparent->i_children )
            {
                    return p_grandparent->pp_children[i+1];
            }
            /* Not found at root */
            if( p_grandparent == p_root )
            {
                return NULL;
            }
            else
            {
                p_parent = p_grandparent;
                p_grandparent = p_parent->p_parent;
            }
        }
    }
    /* We reached root */
    return NULL;
}
