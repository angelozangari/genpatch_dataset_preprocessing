}
static int Filter( video_splitter_t *p_splitter, picture_t *pp_dst[], picture_t *p_src )
{
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    if( video_splitter_NewPicture( p_splitter, pp_dst ) )
    {
        picture_Release( p_src );
        return VLC_EGENERIC;
    }
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            wall_output_t *p_output = &p_sys->pp_output[x][y];
            if( !p_output->b_active )
                continue;
            picture_t *p_dst = pp_dst[p_output->i_output];
            /* */
            picture_t tmp = *p_src;
            for( int i = 0; i < tmp.i_planes; i++ )
            {
                plane_t *p0 = &tmp.p[0];
                plane_t *p = &tmp.p[i];
                const int i_y = p_output->i_top  * p->i_visible_pitch / p0->i_visible_pitch;
                const int i_x = p_output->i_left * p->i_visible_lines / p0->i_visible_lines;
                p->p_pixels += i_y * p->i_pitch + ( i_x - (i_x % p->i_pixel_pitch));
            }
            picture_Copy( p_dst, &tmp );
        }
    }
    picture_Release( p_src );
    return VLC_SUCCESS;
}
