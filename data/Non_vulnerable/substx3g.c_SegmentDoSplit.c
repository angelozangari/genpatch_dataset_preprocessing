}
static void SegmentDoSplit( segment_t *p_segment, uint16_t i_start, uint16_t i_end,
                            segment_t **pp_segment_left,
                            segment_t **pp_segment_middle,
                            segment_t **pp_segment_right )
{
    segment_t *p_segment_left = *pp_segment_left;
    segment_t *p_segment_right = *pp_segment_right;
    segment_t *p_segment_middle = *pp_segment_middle;
    p_segment_left = p_segment_middle = p_segment_right = NULL;
    if ( (p_segment->i_size - i_start < 1) || (p_segment->i_size - i_end < 1) )
        return;
    if ( i_start > 0 )
    {
        p_segment_left = calloc( 1, sizeof(segment_t) );
        if ( !p_segment_left ) goto error;
        memcpy( &p_segment_left->styles, &p_segment->styles, sizeof(segment_style_t) );
        p_segment_left->psz_string = str8indup( p_segment->psz_string, 0, i_start );
        p_segment_left->i_size = str8len( p_segment_left->psz_string );
    }
    p_segment_middle = calloc( 1, sizeof(segment_t) );
    if ( !p_segment_middle ) goto error;
    memcpy( &p_segment_middle->styles, &p_segment->styles, sizeof(segment_style_t) );
    p_segment_middle->psz_string = str8indup( p_segment->psz_string, i_start, i_end - i_start + 1 );
    p_segment_middle->i_size = str8len( p_segment_middle->psz_string );
    if ( i_end < (p_segment->i_size - 1) )
    {
        p_segment_right = calloc( 1, sizeof(segment_t) );
        if ( !p_segment_right ) goto error;
        memcpy( &p_segment_right->styles, &p_segment->styles, sizeof(segment_style_t) );
        p_segment_right->psz_string = str8indup( p_segment->psz_string, i_end + 1, p_segment->i_size - i_end - 1 );
        p_segment_right->i_size = str8len( p_segment_right->psz_string );
    }
    if ( p_segment_left ) p_segment_left->p_next = p_segment_middle;
    if ( p_segment_right ) p_segment_middle->p_next = p_segment_right;
    *pp_segment_left = p_segment_left;
    *pp_segment_middle = p_segment_middle;
    *pp_segment_right = p_segment_right;
    return;
error:
    SegmentFree( p_segment_left );
    SegmentFree( p_segment_middle );
    SegmentFree( p_segment_right );
}
