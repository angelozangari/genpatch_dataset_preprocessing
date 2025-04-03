}
int playlist_NodeInsert( playlist_t *p_playlist,
                         playlist_item_t *p_item,
                         playlist_item_t *p_parent,
                         int i_position )
{
    PL_ASSERT_LOCKED;
    (void)p_playlist;
    assert( p_parent && p_parent->i_children != -1 );
    if( i_position == -1 ) i_position = p_parent->i_children ;
    assert( i_position <= p_parent->i_children);
    INSERT_ELEM( p_parent->pp_children,
                 p_parent->i_children,
                 i_position,
                 p_item );
    p_item->p_parent = p_parent;
    return VLC_SUCCESS;
}
