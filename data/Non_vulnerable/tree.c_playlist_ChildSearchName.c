 */
playlist_item_t *playlist_ChildSearchName( playlist_item_t *p_node,
                                           const char *psz_search )
{
    playlist_t * p_playlist = p_node->p_playlist; /* For assert_locked */
    PL_ASSERT_LOCKED;
    int i;
    if( p_node->i_children < 0 )
    {
         return NULL;
    }
    for( i = 0 ; i< p_node->i_children; i++ )
    {
        if( !strcmp( p_node->pp_children[i]->p_input->psz_name, psz_search ) )
        {
            return p_node->pp_children[i];
        }
    }
    return NULL;
}
