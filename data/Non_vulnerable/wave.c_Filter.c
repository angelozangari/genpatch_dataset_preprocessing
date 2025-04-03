 *****************************************************************************/
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic )
{
    picture_t *p_outpic;
    int i_index;
    double f_angle;
    mtime_t new_date = mdate();
    if( !p_pic ) return NULL;
    p_outpic = filter_NewPicture( p_filter );
    if( !p_outpic )
    {
        picture_Release( p_pic );
        return NULL;
    }
    p_filter->p_sys->f_angle += (new_date - p_filter->p_sys->last_date) / 200000.0;
    p_filter->p_sys->last_date = new_date;
    f_angle = p_filter->p_sys->f_angle;
    for( i_index = 0 ; i_index < p_pic->i_planes ; i_index++ )
    {
        int i_line, i_num_lines, i_visible_pitch, i_pixel_pitch, i_offset,
            i_visible_pixels;
        uint8_t black_pixel;
        uint8_t *p_in, *p_out;
        p_in = p_pic->p[i_index].p_pixels;
        p_out = p_outpic->p[i_index].p_pixels;
        i_num_lines = p_pic->p[i_index].i_visible_lines;
        i_visible_pitch = p_pic->p[i_index].i_visible_pitch;
        i_pixel_pitch = p_pic->p[i_index].i_pixel_pitch;
        switch( p_filter->fmt_in.video.i_chroma )
        {
            CASE_PACKED_YUV_422
                // Quick hack to fix u/v inversion occuring with 2 byte pixel pitch
                i_pixel_pitch *= 2;
                break;
        }
        i_visible_pixels = i_visible_pitch/i_pixel_pitch;
        black_pixel = ( p_pic->i_planes > 1 && i_index == Y_PLANE ) ? 0x00
                                                                    : 0x80;
        /* Ok, we do 3 times the sin() calculation for each line. So what ? */
        for( i_line = 0 ; i_line < i_num_lines ; i_line++ )
        {
            /* Calculate today's offset, don't go above 1/20th of the screen */
            i_offset = (int)( (double)(i_visible_pixels)
                         * sin( f_angle + 10.0 * (double)i_line
                                               / (double)i_num_lines )
                         / 20.0 )*i_pixel_pitch;
            if( i_offset )
            {
                if( i_offset < 0 )
                {
                    memcpy( p_out, p_in - i_offset,
                                i_visible_pitch + i_offset );
                    p_in += p_pic->p[i_index].i_pitch;
                    p_out += p_outpic->p[i_index].i_pitch;
                    memset( p_out + i_offset, black_pixel, -i_offset );
                }
                else
                {
                    memcpy( p_out + i_offset, p_in,
                                i_visible_pitch - i_offset );
                    memset( p_out, black_pixel, i_offset );
                    p_in += p_pic->p[i_index].i_pitch;
                    p_out += p_outpic->p[i_index].i_pitch;
                }
            }
            else
            {
                memcpy( p_out, p_in, i_visible_pitch );
                p_in += p_pic->p[i_index].i_pitch;
                p_out += p_outpic->p[i_index].i_pitch;
            }
        }
    }
    return CopyInfoAndRelease( p_outpic, p_pic );
}
