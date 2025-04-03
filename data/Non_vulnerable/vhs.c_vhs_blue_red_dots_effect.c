 */
static void vhs_blue_red_dots_effect( filter_t *p_filter, picture_t *p_pic_out ) {
#define BR_DOTS_RATIO 10000
    filter_sys_t *p_sys = p_filter->p_sys;
    for ( int32_t i_dots = 0;
          i_dots < p_sys->i_width[Y_PLANE] * p_sys->i_height[Y_PLANE] / BR_DOTS_RATIO;
          i_dots++) {
        uint32_t i_length = (unsigned)vlc_mrand48()
                          % __MAX( 1, p_sys->i_width[Y_PLANE] / 30 ) + 1;
        uint16_t i_x = (unsigned)vlc_mrand48()
                     % __MAX( 1, p_sys->i_width[Y_PLANE] - i_length );
        uint16_t i_y = (unsigned)vlc_mrand48() % p_sys->i_height[Y_PLANE];
        bool b_color = ( ( (unsigned)vlc_mrand48() % 2 ) == 0);
        for ( int32_t i_p = 0; i_p < p_sys->i_planes; i_p++ ) {
            uint32_t i_pix_ofs = i_y
                               * p_pic_out->p[i_p].i_visible_lines
                               / p_sys->i_height[Y_PLANE]
                               * p_pic_out->p[i_p].i_pitch
                               + i_x
                               * p_pic_out->p[i_p].i_pixel_pitch;
            uint32_t i_length_in_plane = i_length
                                       * p_pic_out->p[i_p].i_visible_pitch
                                       / p_pic_out->p[Y_PLANE].i_visible_pitch;
            switch ( i_p ) {
              case Y_PLANE:
                memset( &p_pic_out->p[i_p].p_pixels[i_pix_ofs], 127,
                        i_length_in_plane );
                break;
              case U_PLANE:
                memset( &p_pic_out->p[i_p].p_pixels[i_pix_ofs],
                        (b_color?255:0),
                        i_length_in_plane );
                break;
              case V_PLANE:
                memset( &p_pic_out->p[i_p].p_pixels[i_pix_ofs],
                        (b_color?0:255),
                        i_length_in_plane );
                break;
            }
        }
    }
}
