 */
int playlist_NodeEmpty( playlist_t *p_playlist, playlist_item_t *p_root,
                        bool b_delete_items )
{
    PL_ASSERT_LOCKED;
    int i;
    if( p_root->i_children == -1 )
    {
        return VLC_EGENERIC;
    }
    /* Delete the children */
    for( i =  p_root->i_children-1 ; i >= 0 ;i-- )
    {
        if( p_root->pp_children[i]->i_children > -1 )
        {
            playlist_NodeDelete( p_playlist, p_root->pp_children[i],
                                 b_delete_items , false );
        }
        else if( b_delete_items )
        {
            /* Delete the item here */
            playlist_DeleteFromItemId( p_playlist,
                                       p_root->pp_children[i]->i_id );
        }
    }
    return VLC_SUCCESS;
}
