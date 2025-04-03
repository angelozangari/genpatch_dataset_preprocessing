 **/
playlist_item_t *GetNextItem( playlist_t *p_playlist,
                              playlist_item_t *p_root,
                              playlist_item_t *p_item )
{
    /* If the item is NULL, return the firt child of root */
    if( p_item == NULL )
    {
        if( p_root->i_children > 0 )
            return p_root->pp_children[0];
        else
            return NULL;
    }
    /* Node with children, get the first one */
    if( p_item->i_children > 0 )
        return p_item->pp_children[0];
    playlist_item_t* p_parent = p_item->p_parent;
    for( int i = 0 ; i < p_parent->i_children ; i++ )
    {
        if( p_parent->pp_children[i] == p_item )
        {
            // Return the next children
            if( i + 1 < p_parent->i_children )
                return p_parent->pp_children[i+1];
            // We are the least one, so try to have uncles
            else
            {
                PL_DEBUG2( "Current item is the last of the node,"
                           "looking for uncle from %s",
                            p_parent->p_input->psz_name );
                if( p_parent == p_root )
                {
                    PL_DEBUG2( "already at root" );
                    return NULL;
                }
                else
                    return GetNextUncle( p_playlist, p_item, p_root );
            }
        }
    }
    return NULL;
}
