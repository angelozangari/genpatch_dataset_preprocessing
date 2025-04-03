 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_block;
    int64_t     i_offset;
    unsigned    i_frames = p_sys->i_block_frames;
    i_offset = stream_Tell( p_demux->s );
    if( p_sys->i_data_size > 0 &&
        i_offset >= p_sys->i_data_offset + p_sys->i_data_size )
    {
        /* EOF */
        return 0;
    }
    p_block = stream_Block( p_demux->s, p_sys->i_frame_size * i_frames );
    if( p_block == NULL )
    {
        msg_Warn( p_demux, "cannot read data" );
        return 0;
    }
    i_frames = p_block->i_buffer / p_sys->i_frame_size;
    p_block->i_dts = p_block->i_pts = date_Get( &p_sys->pts );
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block->i_pts );
    es_out_Send( p_demux->out, p_sys->p_es, p_block );
    date_Increment( &p_sys->pts, i_frames * FRAME_LENGTH );
    return 1;
}
