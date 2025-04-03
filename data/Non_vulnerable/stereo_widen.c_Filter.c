 *****************************************************************************/
static block_t *Filter( filter_t *p_filter, block_t *p_block )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    float *p_out = (float *)p_block->p_buffer;
    float *pf_read;
    for (unsigned i = p_block->i_nb_samples; i > 0; i--)
    {
        pf_read = p_sys->pf_write + 2;
        /* if at end of buffer put read ptr at begin */
        if( pf_read >= p_sys->pf_ringbuf + p_sys->i_len )
            pf_read = p_sys->pf_ringbuf;
        float left  = p_out[0];
        float right = p_out[1];
        *(p_out++) = p_sys->f_dry_mix * left  - p_sys->f_crossfeed * right
                        - p_sys->f_feedback * pf_read[1];
        *(p_out++) = p_sys->f_dry_mix * right - p_sys->f_crossfeed * left
                        - p_sys->f_feedback * pf_read[0];
        *(p_sys->pf_write++) = left ;
        *(p_sys->pf_write++) = right;
        /* if at end of buffer place pf_write at begin */
        if( p_sys->pf_write  == p_sys->pf_ringbuf + p_sys->i_len )
            p_sys->pf_write  =  p_sys->pf_ringbuf;
    }
    return p_block;
}
