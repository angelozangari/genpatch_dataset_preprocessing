 */
playlist_item_t *playlist_GetNextLeaf( playlist_t *p_playlist,
                                       playlist_item_t *p_root,
                                       playlist_item_t *p_item,
                                       bool b_ena, bool b_unplayed )
{
    PL_ASSERT_LOCKED;
    playlist_item_t *p_next;
    assert( p_root && p_root->i_children != -1 );
    PL_DEBUG2( "finding next of %s within %s",
               PLI_NAME( p_item ), PLI_NAME( p_root ) );
    /* Now, walk the tree until we find a suitable next item */
    p_next = p_item;
    while( 1 )
    {
        bool b_ena_ok = true, b_unplayed_ok = true;
        p_next = GetNextItem( p_playlist, p_root, p_next );
        if( !p_next || p_next == p_root )
            break;
        if( p_next->i_children == -1 )
        {
            if( b_ena && p_next->i_flags & PLAYLIST_DBL_FLAG )
                b_ena_ok = false;
            if( b_unplayed && p_next->p_input->i_nb_played != 0 )
                b_unplayed_ok = false;
            if( b_ena_ok && b_unplayed_ok ) break;
        }
    }
    if( p_next == NULL ) PL_DEBUG2( "at end of node" );
    return p_next;
}
