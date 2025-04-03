}
static void GetPixels( uint8_t *pp_pixel[4], int pi_pitch[4],
                       const vlc_chroma_description_t *desc,
                       const video_format_t *fmt,
                       const picture_t *p_picture, unsigned planes,
                       bool b_swap_uv )
{
    unsigned i = 0;
    if( planes > (unsigned)p_picture->i_planes )
        planes = p_picture->i_planes;
    assert( !b_swap_uv || planes >= 3 );
    for( ; i < planes; i++ )
    {
        const plane_t *p = p_picture->p + i;
        if( b_swap_uv && (i == 1 || i== 2) )
            p = p_picture->p + 3 - i;
        pp_pixel[i] = p->p_pixels
            + (((fmt->i_x_offset * desc->p[i].w.num) / desc->p[i].w.den)
                * p->i_pixel_pitch)
            + (((fmt->i_y_offset * desc->p[i].h.num) / desc->p[i].h.den)
                * p->i_pitch);
        pi_pitch[i] = p->i_pitch;
    }
    for( ; i < 4; i++ )
    {
        pp_pixel[i] = NULL;
        pi_pitch[i] = 0;
    }
}
