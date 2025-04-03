 */
playlist_item_t *playlist_GetPrevLeaf( playlist_t *p_playlist,
                                       playlist_item_t *p_root,
                                       playlist_item_t *p_item,
                                       bool b_ena, bool b_unplayed )
{
    PL_ASSERT_LOCKED;
    playlist_item_t *p_prev;
    PL_DEBUG2( "finding previous of %s within %s", PLI_NAME( p_item ),
                                                   PLI_NAME( p_root ) );
    assert( p_root && p_root->i_children != -1 );
    /* Now, walk the tree until we find a suitable previous item */
    p_prev = p_item;
    while( 1 )
    {
        bool b_ena_ok = true, b_unplayed_ok = true;
        p_prev = GetPrevItem( p_playlist, p_root, p_prev );
        if( !p_prev || p_prev == p_root )
            break;
        if( p_prev->i_children == -1 )
        {
            if( b_ena && p_prev->i_flags & PLAYLIST_DBL_FLAG )
                b_ena_ok = false;
            if( b_unplayed && p_prev->p_input->i_nb_played != 0 )
                b_unplayed_ok = false;
            if( b_ena_ok && b_unplayed_ok ) break;
        }
    }
    if( p_prev == NULL ) PL_DEBUG2( "at beginning of node" );
    return p_prev;
}
