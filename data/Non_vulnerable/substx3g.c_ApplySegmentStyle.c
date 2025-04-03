   to the start & end offsets */
static void ApplySegmentStyle( segment_t **pp_segment, const uint16_t i_absstart,
                               const uint16_t i_absend, const segment_style_t *p_styles )
{
    /* find the matching segment */
    uint16_t i_curstart = 0;
    segment_t *p_prev = NULL;
    segment_t *p_cur = *pp_segment;
    while ( p_cur )
    {
        uint16_t i_curend = i_curstart + p_cur->i_size - 1;
        if ( (i_absstart >= i_curstart) && (i_absend <= i_curend) )
        {
            /* segment found */
            if ( !SegmentSplit( p_prev, &p_cur, i_absstart - i_curstart,
                                i_absend - i_curstart, p_styles ) )
                return;
            if ( !p_prev ) *pp_segment = p_cur;
            break;
        }
        else
        {
            i_curstart += p_cur->i_size;
            p_prev = p_cur;
            p_cur = p_cur->p_next;
        }
    }
}
