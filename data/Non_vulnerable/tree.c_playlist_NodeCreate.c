 */
playlist_item_t * playlist_NodeCreate( playlist_t *p_playlist,
                                       const char *psz_name,
                                       playlist_item_t *p_parent, int i_pos,
                                       int i_flags, input_item_t *p_input )
{
    input_item_t *p_new_input = NULL;
    playlist_item_t *p_item;
    PL_ASSERT_LOCKED;
    if( !psz_name ) psz_name = _("Undefined");
    if( !p_input )
        p_new_input = input_item_NewWithType( NULL, psz_name, 0, NULL, 0, -1,
                                              ITEM_TYPE_NODE );
    p_item = playlist_ItemNewFromInput( p_playlist,
                                        p_input ? p_input : p_new_input );
    if( p_new_input )
        vlc_gc_decref( p_new_input );
    if( p_item == NULL )  return NULL;
    p_item->i_children = 0;
    ARRAY_APPEND(p_playlist->all_items, p_item);
    if( p_parent != NULL )
        playlist_NodeInsert( p_playlist, p_item, p_parent,
                             i_pos == PLAYLIST_END ? -1 : i_pos );
    playlist_SendAddNotify( p_playlist, p_item->i_id,
                            p_parent ? p_parent->i_id : -1,
                            !( i_flags & PLAYLIST_NO_REBUILD ));
    p_item->i_flags |= i_flags;
    return p_item;
}
