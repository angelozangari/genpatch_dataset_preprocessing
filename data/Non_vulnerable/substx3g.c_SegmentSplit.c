}
static bool SegmentSplit( segment_t *p_prev, segment_t **pp_segment,
                          const uint16_t i_start, const uint16_t i_end,
                          const segment_style_t *p_styles )
{
    segment_t *p_segment_left = NULL, *p_segment_middle = NULL, *p_segment_right = NULL;
    if ( (*pp_segment)->i_size == 0 ) return false;
    if ( i_start > i_end ) return false;
    if ( (size_t)(i_end - i_start) > (*pp_segment)->i_size - 1 ) return false;
    if ( i_end > (*pp_segment)->i_size - 1 ) return false;
    SegmentDoSplit( *pp_segment, i_start, i_end, &p_segment_left, &p_segment_middle, &p_segment_right );
    if ( !p_segment_middle )
    {
        /* Failed */
        SegmentFree( p_segment_left );
        SegmentFree( p_segment_right );
        return false;
    }
    segment_t *p_next = (*pp_segment)->p_next;
    SegmentFree( *pp_segment );
    *pp_segment = ( p_segment_left ) ? p_segment_left : p_segment_middle ;
    if ( p_prev ) p_prev->p_next = *pp_segment;
    if ( p_segment_right )
        p_segment_right->p_next = p_next;
    else
        p_segment_middle->p_next = p_next;
    p_segment_middle->styles = *p_styles;
    return true;
}
