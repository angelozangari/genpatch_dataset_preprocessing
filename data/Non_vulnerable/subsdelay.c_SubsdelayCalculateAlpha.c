 *****************************************************************************/
static int SubsdelayCalculateAlpha( filter_t *p_filter, int i_overlapping, int i_source_alpha )
{
    int i_new_alpha;
    int i_min_alpha;
    i_min_alpha = p_filter->p_sys->i_min_alpha;
    if( i_overlapping > p_filter->p_sys->i_overlap - 1)
    {
        i_overlapping = p_filter->p_sys->i_overlap - 1;
    }
    switch ( p_filter->p_sys->i_overlap )
    {
    case 1:
        i_new_alpha = 255;
        break;
    case 2:
        i_new_alpha = 255 - i_overlapping * ( 255 - i_min_alpha );
        break;
    case 3:
        i_new_alpha = 255 - i_overlapping * ( 255 - i_min_alpha ) / 2;
        break;
    default:
        i_new_alpha = 255 - i_overlapping * ( 255 - i_min_alpha ) / 3;
        break;
    }
    return ( i_source_alpha * i_new_alpha ) / 255;
}
