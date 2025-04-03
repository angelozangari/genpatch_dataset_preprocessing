 */
int playlist_NodeRemoveItem( playlist_t *p_playlist,
                        playlist_item_t *p_item,
                        playlist_item_t *p_parent )
{
    PL_ASSERT_LOCKED;
    (void)p_playlist;
    int ret = VLC_EGENERIC;
    for(int i= 0; i< p_parent->i_children ; i++ )
    {
        if( p_parent->pp_children[i] == p_item )
        {
            REMOVE_ELEM( p_parent->pp_children, p_parent->i_children, i );
            ret = VLC_SUCCESS;
        }
    }
    if( ret == VLC_SUCCESS ) {
        assert( p_item->p_parent == p_parent );
        p_item->p_parent = NULL;
    }
    return ret;
}
