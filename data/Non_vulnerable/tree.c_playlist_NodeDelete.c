 */
int playlist_NodeDelete( playlist_t *p_playlist, playlist_item_t *p_root,
                         bool b_delete_items, bool b_force )
{
    PL_ASSERT_LOCKED;
    /* Delete the children */
    for( int i = p_root->i_children - 1 ; i >= 0; i-- )
        if( b_delete_items || p_root->pp_children[i]->i_children >= 0 )
            playlist_NodeDelete( p_playlist, p_root->pp_children[i],
                                 b_delete_items, b_force );
    /* Delete the node */
    if( p_root->i_flags & PLAYLIST_RO_FLAG && !b_force )
        return VLC_SUCCESS;
    pl_priv(p_playlist)->b_reset_currently_playing = true;
    int i;
    var_SetInteger( p_playlist, "playlist-item-deleted", p_root->i_id );
    ARRAY_BSEARCH( p_playlist->all_items, ->i_id, int, p_root->i_id, i );
    if( i != -1 )
        ARRAY_REMOVE( p_playlist->all_items, i );
    if( p_root->i_children == -1 ) {
        ARRAY_BSEARCH( p_playlist->items,->i_id, int, p_root->i_id, i );
        if( i != -1 )
            ARRAY_REMOVE( p_playlist->items, i );
    }
    /* Check if it is the current item */
    if( get_current_status_item( p_playlist ) == p_root )
    {
        /* Stop */
        playlist_Control( p_playlist, PLAYLIST_STOP, pl_Locked );
        msg_Info( p_playlist, "stopping playback" );
        /* This item can't be the next one to be played ! */
        set_current_status_item( p_playlist, NULL );
    }
    ARRAY_BSEARCH( p_playlist->current,->i_id, int, p_root->i_id, i );
    if( i != -1 )
        ARRAY_REMOVE( p_playlist->current, i );
    PL_DEBUG( "deleting item `%s'", p_root->p_input->psz_name );
    /* Remove the item from its parent */
    if( p_root->p_parent )
        playlist_NodeRemoveItem( p_playlist, p_root, p_root->p_parent );
    playlist_ItemRelease( p_root );
    return VLC_SUCCESS;
}
