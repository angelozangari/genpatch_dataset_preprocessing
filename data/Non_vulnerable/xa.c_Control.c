 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys  = p_demux->p_sys;
    return demux_vaControlHelper( p_demux->s, p_sys->i_data_offset,
                                   p_sys->i_data_size ? p_sys->i_data_offset
                                   + p_sys->i_data_size : -1,
                                   p_sys->i_bitrate, p_sys->i_frame_size,
                                   i_query, args );
}
