/* Recursively search the tree for previous item */
playlist_item_t *GetPrevItem( playlist_t *p_playlist,
                              playlist_item_t *p_root,
                              playlist_item_t *p_item )
{
    playlist_item_t *p_parent;
    int i;
    /* Node with children, get the last one */
    if( p_item && p_item->i_children > 0 )
        return p_item->pp_children[p_item->i_children - 1];
    /* Last child of its parent ? */
    if( p_item != NULL )
        p_parent = p_item->p_parent;
    else
    {
        msg_Err( p_playlist, "Get the last one" );
        abort();
    };
    for( i = p_parent->i_children -1 ; i >= 0 ;  i-- )
    {
        if( p_parent->pp_children[i] == p_item )
        {
            if( i-1 < 0 )
            {
               /* Was already the first sibling. Look for uncles */
                PL_DEBUG2( "current item is the first of its node,"
                           "looking for uncle from %s",
                           p_parent->p_input->psz_name );
                if( p_parent == p_root )
                {
                    PL_DEBUG2( "already at root" );
                    return NULL;
                }
                return GetPrevUncle( p_playlist, p_item, p_root );
            }
            else
            {
                return p_parent->pp_children[i-1];
            }
        }
    }
    return NULL;
}
