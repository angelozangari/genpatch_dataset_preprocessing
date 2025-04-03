} chapters_array_t;
static seekpoint_t * getChapterEntry( unsigned int i_index, chapters_array_t *p_array )
{
    if ( i_index > 4096 ) return NULL;
    if ( i_index >= p_array->i_size )
    {
        unsigned int i_newsize = p_array->i_size;
        while( i_index >= i_newsize ) i_newsize += 50;
        if ( !p_array->pp_chapters )
        {
            p_array->pp_chapters = calloc( i_newsize, sizeof( seekpoint_t * ) );
            if ( !p_array->pp_chapters ) return NULL;
            p_array->i_size = i_newsize;
        } else {
            seekpoint_t **tmp = calloc( i_newsize, sizeof( seekpoint_t * ) );
            if ( !tmp ) return NULL;
            memcpy( tmp, p_array->pp_chapters, p_array->i_size * sizeof( seekpoint_t * ) );
            free( p_array->pp_chapters );
            p_array->pp_chapters = tmp;
            p_array->i_size = i_newsize;
        }
    }
    if ( !p_array->pp_chapters[i_index] )
        p_array->pp_chapters[i_index] = vlc_seekpoint_New();
    return p_array->pp_chapters[i_index];
}
