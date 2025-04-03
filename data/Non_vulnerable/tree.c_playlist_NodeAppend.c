 */
int playlist_NodeAppend( playlist_t *p_playlist,
                         playlist_item_t *p_item,
                         playlist_item_t *p_parent )
{
    return playlist_NodeInsert( p_playlist, p_item, p_parent, -1 );
}
