 *****************************************************************************/
static void YUY2_I422( filter_t *p_filter, picture_t *p_source,
                                           picture_t *p_dest )
{
    uint8_t *p_line = p_source->p->p_pixels;
    uint8_t *p_y = p_dest->Y_PIXELS;
    uint8_t *p_u = p_dest->U_PIXELS;
    uint8_t *p_v = p_dest->V_PIXELS;
    int i_x, i_y;
    const int i_dest_margin = p_dest->p[0].i_pitch
                                 - p_dest->p[0].i_visible_pitch;
    const int i_dest_margin_c = p_dest->p[1].i_pitch
                                 - p_dest->p[1].i_visible_pitch;
    const int i_source_margin = p_source->p->i_pitch
                               - p_source->p->i_visible_pitch;
    for( i_y = p_filter->fmt_out.video.i_height ; i_y-- ; )
    {
        for( i_x = p_filter->fmt_out.video.i_width / 8 ; i_x-- ; )
        {
#define C_YUYV_YUV422( p_line, p_y, p_u, p_v )      \
            *p_y++ = *p_line++; *p_u++ = *p_line++; \
            *p_y++ = *p_line++; *p_v++ = *p_line++
            C_YUYV_YUV422( p_line, p_y, p_u, p_v );
            C_YUYV_YUV422( p_line, p_y, p_u, p_v );
            C_YUYV_YUV422( p_line, p_y, p_u, p_v );
            C_YUYV_YUV422( p_line, p_y, p_u, p_v );
        }
        for( i_x = ( p_filter->fmt_out.video.i_width % 8 ) / 2; i_x-- ; )
        {
            C_YUYV_YUV422( p_line, p_y, p_u, p_v );
        }
        p_line += i_source_margin;
        p_y += i_dest_margin;
        p_u += i_dest_margin_c;
        p_v += i_dest_margin_c;
    }
}
