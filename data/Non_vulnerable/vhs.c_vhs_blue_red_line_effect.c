 */
static int vhs_blue_red_line_effect( filter_t *p_filter, picture_t *p_pic_out ) {
    filter_sys_t *p_sys = p_filter->p_sys;
#define BR_LINES_GENERATOR_PERIOD ( TIME_UNIT_PER_S * 50 )
#define BR_LINES_DURATION         ( TIME_UNIT_PER_S * 1/50 )
    /* generate new blue or red lines */
    if ( p_sys->i_BR_line_trigger <= p_sys->i_cur_time ) {
        for ( uint32_t i_b = 0; i_b < MAX_BLUE_RED_LINES; i_b++ )
            if (p_sys->p_BR_lines[i_b] == NULL) {
                /* allocate data */
                p_sys->p_BR_lines[i_b] = calloc( 1, sizeof(blue_red_line_t) );
                if ( unlikely( !p_sys->p_BR_lines[i_b] ) )
                    return VLC_ENOMEM;
                /* set random parameters */
                p_sys->p_BR_lines[i_b]->i_offset = (unsigned)vlc_mrand48()
                                                 % __MAX( 1, p_sys->i_height[Y_PLANE] - 10 )
                                                 + 5;
                p_sys->p_BR_lines[i_b]->b_blue_red = (unsigned)vlc_mrand48() & 0x01;
                p_sys->p_BR_lines[i_b]->i_stop_trigger = p_sys->i_cur_time
                                                       + (uint64_t)vlc_mrand48() % BR_LINES_DURATION
                                                       + BR_LINES_DURATION / 2;
                break;
            }
        p_sys->i_BR_line_trigger = p_sys->i_cur_time
                                 + (uint64_t)vlc_mrand48() % BR_LINES_GENERATOR_PERIOD
                                 + BR_LINES_GENERATOR_PERIOD / 2;
    }
    /* manage and apply current blue/red lines */
    for ( uint8_t i_b = 0; i_b < MAX_BLUE_RED_LINES; i_b++ )
        if ( p_sys->p_BR_lines[i_b] ) {
            /* remove outdated ones */
            if ( p_sys->p_BR_lines[i_b]->i_stop_trigger <= p_sys->i_cur_time ) {
                FREENULL( p_sys->p_BR_lines[i_b] );
                continue;
            }
            /* otherwise apply */
            for ( int32_t i_p=0; i_p < p_sys->i_planes; i_p++ ) {
                uint32_t i_pix_ofs = p_sys->p_BR_lines[i_b]->i_offset
                                   * p_pic_out->p[i_p].i_visible_lines
                                   / p_sys->i_height[Y_PLANE]
                                   * p_pic_out->p[i_p].i_pitch;
                switch ( i_p ) {
                  case Y_PLANE:
                    memset( &p_pic_out->p[i_p].p_pixels[i_pix_ofs], 127,
                            p_pic_out->p[i_p].i_visible_pitch);
                    break;
                  case U_PLANE:
                    memset( &p_pic_out->p[i_p].p_pixels[i_pix_ofs],
                            (p_sys->p_BR_lines[i_b]->b_blue_red?255:0),
                            p_pic_out->p[i_p].i_visible_pitch);
                    break;
                  case V_PLANE:
                    memset( &p_pic_out->p[i_p].p_pixels[i_pix_ofs],
                            (p_sys->p_BR_lines[i_b]->b_blue_red?0:255),
                            p_pic_out->p[i_p].i_visible_pitch);
                    break;
                }
            }
        }
    return VLC_SUCCESS;
}
