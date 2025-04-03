 */
static void vhs_free_allocated_data( filter_t *p_filter ) {
    filter_sys_t *p_sys = p_filter->p_sys;
    for ( uint32_t i_b = 0; i_b < MAX_BLUE_RED_LINES; i_b++ )
        FREENULL( p_sys->p_BR_lines[i_b] );
    p_sys->i_planes = 0;
    FREENULL( p_sys->i_height );
    FREENULL( p_sys->i_width );
    FREENULL( p_sys->i_visible_pitch );
}
