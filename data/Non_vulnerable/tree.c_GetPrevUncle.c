}
playlist_item_t *GetPrevUncle( playlist_t *p_playlist, playlist_item_t *p_item,
                               playlist_item_t *p_root )
{
    playlist_item_t *p_parent = p_item->p_parent;
    playlist_item_t *p_grandparent;
    bool b_found = false;
    (void)p_playlist;
    if( p_parent != NULL )
    {
        p_grandparent = p_parent->p_parent;
        while( 1 )
        {
            int i;
            for( i = p_grandparent->i_children -1 ; i >= 0; i-- )
            {
                if( p_parent == p_grandparent->pp_children[i] )
                {
                    b_found = true;
                    break;
                }
            }
            if( b_found && i - 1 > 0 )
            {
                return p_grandparent->pp_children[i-1];
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
