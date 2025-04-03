#define SAMPLES_BUFFER 1000
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_block;
    int64_t     i;
    if( p_sys->i_silence_countdown == 0 )
    {
        int64_t i_offset = stream_Tell( p_demux->s );
        if( i_offset >= p_sys->i_block_end )
        {
            if( ReadBlockHeader( p_demux ) != VLC_SUCCESS )
                return 0;
            return 1;
        }
        i = ( p_sys->i_block_end - i_offset )
            / p_sys->fmt.audio.i_bytes_per_frame;
        if( i > SAMPLES_BUFFER )
            i = SAMPLES_BUFFER;
        p_block = stream_Block( p_demux->s,
                                p_sys->fmt.audio.i_bytes_per_frame * i );
        if( p_block == NULL )
        {
            msg_Warn( p_demux, "cannot read data" );
            return 0;
        }
    }
    else
    {   /* emulates silence from the stream */
        i = p_sys->i_silence_countdown;
        if( i > SAMPLES_BUFFER )
            i = SAMPLES_BUFFER;
        p_block = block_Alloc( i );
        if( p_block == NULL )
            return VLC_ENOMEM;
        memset( p_block->p_buffer, 0, i );
        p_sys->i_silence_countdown -= i;
    }
    p_block->i_dts = p_block->i_pts = VLC_TS_0 + date_Get( &p_sys->pts );
    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_block->i_pts );
    es_out_Send( p_demux->out, p_sys->p_es, p_block );
    date_Increment( &p_sys->pts, p_sys->fmt.audio.i_frame_length * i );
    return 1;
}
