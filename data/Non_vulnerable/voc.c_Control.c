 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    return demux_vaControlHelper( p_demux->s, p_sys->i_block_start,
                                   p_sys->i_block_end,
                                   p_sys->fmt.i_bitrate,
                                   p_sys->fmt.audio.i_blockalign,
                                   i_query, args );
}
