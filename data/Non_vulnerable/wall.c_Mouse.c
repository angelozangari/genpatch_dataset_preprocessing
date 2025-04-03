}
static int Mouse( video_splitter_t *p_splitter, vlc_mouse_t *p_mouse,
                  int i_index,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    VLC_UNUSED(p_old);
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            wall_output_t *p_output = &p_sys->pp_output[x][y];
            if( p_output->b_active && p_output->i_output == i_index )
            {
                *p_mouse = *p_new;
                p_mouse->i_x += p_output->i_left;
                p_mouse->i_y += p_output->i_top;
                return VLC_SUCCESS;
            }
        }
    }
    assert(0);
    return VLC_EGENERIC;
}
}
static int Mouse( video_splitter_t *p_splitter, vlc_mouse_t *p_mouse,
                  int i_index,
                  const vlc_mouse_t *p_old, const vlc_mouse_t *p_new )
{
    VLC_UNUSED(p_old);
    video_splitter_sys_t *p_sys = p_splitter->p_sys;
    for( int y = 0; y < p_sys->i_row; y++ )
    {
        for( int x = 0; x < p_sys->i_col; x++ )
        {
            wall_output_t *p_output = &p_sys->pp_output[x][y];
            if( p_output->b_active && p_output->i_output == i_index )
            {
                *p_mouse = *p_new;
                p_mouse->i_x += p_output->i_left;
                p_mouse->i_y += p_output->i_top;
                return VLC_SUCCESS;
            }
        }
    }
