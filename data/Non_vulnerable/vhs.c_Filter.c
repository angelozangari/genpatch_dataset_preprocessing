 */
static picture_t *Filter( filter_t *p_filter, picture_t *p_pic_in ) {
    if( unlikely( !p_pic_in || !p_filter) )
        return NULL;
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_pic_out = filter_NewPicture( p_filter );
    if( unlikely( !p_pic_out ) ) {
        picture_Release( p_pic_in );
        return NULL;
    }
   /*
    * manage time
    */
    p_sys->i_last_time = p_sys->i_cur_time;
    p_sys->i_cur_time = NTPtime64();
   /*
    * allocate data
    */
    if ( unlikely( !p_sys->b_init ) )
        if ( unlikely( vhs_allocate_data( p_filter, p_pic_in ) != VLC_SUCCESS ) ) {
            picture_Release( p_pic_in );
            return NULL;
        }
    p_sys->b_init = true;
   /*
    * preset output pic: raw copy src to dst
    */
    picture_CopyPixels(p_pic_out, p_pic_in);
   /*
    * apply effects on picture
    */
    if ( unlikely( vhs_blue_red_line_effect( p_filter, p_pic_out ) != VLC_SUCCESS ) )
        return CopyInfoAndRelease( p_pic_out, p_pic_in );
    if ( unlikely( vhs_sliding_effect(p_filter, p_pic_out ) != VLC_SUCCESS ) )
        return CopyInfoAndRelease( p_pic_out, p_pic_in );
    vhs_blue_red_dots_effect( p_filter, p_pic_out );
    return CopyInfoAndRelease( p_pic_out, p_pic_in );
}
