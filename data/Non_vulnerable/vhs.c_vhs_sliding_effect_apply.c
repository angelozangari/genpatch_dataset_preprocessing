*/
static int vhs_sliding_effect_apply( filter_t *p_filter, picture_t *p_pic_out )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    for ( uint8_t i_p = 0; i_p < p_pic_out->i_planes; i_p++ ) {
        /* first allocate temporary buffer for swap operation */
        uint8_t *p_temp_buf;
        if ( !p_sys->i_sliding_type_duplicate ) {
            p_temp_buf= calloc( p_pic_out->p[i_p].i_lines
                                * p_pic_out->p[i_p].i_pitch, sizeof(uint8_t) );
            if ( unlikely( !p_temp_buf ) )
                return VLC_ENOMEM;
            memcpy( p_temp_buf, p_pic_out->p[i_p].p_pixels,
                    p_pic_out->p[i_p].i_lines * p_pic_out->p[i_p].i_pitch );
        }
        else
            p_temp_buf = p_pic_out->p[i_p].p_pixels;
        /* copy lines to output_pic */
        for ( int32_t i_y = 0; i_y < p_pic_out->p[i_p].i_visible_lines; i_y++ )
        {
            int32_t i_ofs = p_sys->i_offset_ofs + p_sys->i_sliding_ofs;
            if ( ( p_sys->i_sliding_speed == 0 ) || !p_sys->i_sliding_type_duplicate )
                i_ofs += p_sys->i_phase_ofs;
            i_ofs  = MOD( i_ofs / 100, p_sys->i_height[Y_PLANE] );
            i_ofs *= p_pic_out->p[i_p].i_visible_lines;
            i_ofs /= p_sys->i_height[Y_PLANE];
            memcpy( &p_pic_out->p[i_p].p_pixels[ i_y * p_pic_out->p[i_p].i_pitch ],
                    &p_temp_buf[ ( ( i_y + i_ofs ) % p_pic_out->p[i_p].i_visible_lines ) * p_pic_out->p[i_p].i_pitch ],
                    p_pic_out->p[i_p].i_visible_pitch );
        }
        if ( !p_sys->i_sliding_type_duplicate )
            free(p_temp_buf);
    }
    return VLC_SUCCESS;
}
