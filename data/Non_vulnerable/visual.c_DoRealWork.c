}
static block_t *DoRealWork( filter_t *p_filter, block_t *p_in_buf )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    picture_t *p_outpic;
    /* First, get a new picture */
    while( ( p_outpic = vout_GetPicture( p_sys->p_vout ) ) == NULL )
        msleep( VOUT_OUTMEM_SLEEP );
    /* Blank the picture */
    for( int i = 0 ; i < p_outpic->i_planes ; i++ )
    {
        memset( p_outpic->p[i].p_pixels, i > 0 ? 0x80 : 0x00,
                p_outpic->p[i].i_visible_lines * p_outpic->p[i].i_pitch );
    }
    /* We can now call our visualization effects */
    for( int i = 0; i < p_sys->i_effect; i++ )
    {
#define p_effect p_sys->effect[i]
        if( p_effect->pf_run )
        {
            p_effect->pf_run( p_effect, VLC_OBJECT(p_filter),
                              p_in_buf, p_outpic );
        }
#undef p_effect
    }
    p_outpic->date = p_in_buf->i_pts + (p_in_buf->i_length / 2);
    vout_PutPicture( p_sys->p_vout, p_outpic );
    return p_in_buf;
}
