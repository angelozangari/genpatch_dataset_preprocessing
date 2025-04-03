 */
static int vhs_allocate_data( filter_t *p_filter, picture_t *p_pic_in ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    vhs_free_allocated_data( p_filter );
   /*
    * take into account different characteristics for each plane
    */
    p_sys->i_planes = p_pic_in->i_planes;
    p_sys->i_height = calloc( p_sys->i_planes, sizeof(int32_t) );
    p_sys->i_width  = calloc( p_sys->i_planes, sizeof(int32_t) );
    p_sys->i_visible_pitch = calloc( p_sys->i_planes, sizeof(int32_t) );
    if( unlikely( !p_sys->i_height || !p_sys->i_width || !p_sys->i_visible_pitch ) ) {
        vhs_free_allocated_data( p_filter );
        return VLC_ENOMEM;
    }
    for ( int32_t i_p = 0; i_p < p_sys->i_planes; i_p++) {
        p_sys->i_visible_pitch [i_p] = (int) p_pic_in->p[i_p].i_visible_pitch;
        p_sys->i_height[i_p] = (int) p_pic_in->p[i_p].i_visible_lines;
        p_sys->i_width [i_p] = (int) p_pic_in->p[i_p].i_visible_pitch / p_pic_in->p[i_p].i_pixel_pitch;
    }
    return VLC_SUCCESS;
}
