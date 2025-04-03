}
static block_t *TSNew( sout_mux_t *p_mux, ts_stream_t *p_stream,
                       bool b_pcr )
{
    VLC_UNUSED(p_mux);
    block_t *p_pes = p_stream->chain_pes.p_first;
    bool b_new_pes = false;
    bool b_adaptation_field = false;
    int i_payload_max = 184 - ( b_pcr ? 8 : 0 );
    if( p_stream->i_pes_used <= 0 )
    {
        b_new_pes = true;
    }
    int i_payload = __MIN( (int)p_pes->i_buffer - p_stream->i_pes_used,
                       i_payload_max );
    if( b_pcr || i_payload < i_payload_max )
    {
        b_adaptation_field = true;
    }
    block_t *p_ts = block_Alloc( 188 );
    if (b_new_pes && !(p_pes->i_flags & BLOCK_FLAG_NO_KEYFRAME) && p_pes->i_flags & BLOCK_FLAG_TYPE_I)
    {
        p_ts->i_flags |= BLOCK_FLAG_TYPE_I;
    }
    p_ts->i_dts = p_pes->i_dts;
    p_ts->p_buffer[0] = 0x47;
    p_ts->p_buffer[1] = ( b_new_pes ? 0x40 : 0x00 ) |
        ( ( p_stream->i_pid >> 8 )&0x1f );
    p_ts->p_buffer[2] = p_stream->i_pid & 0xff;
    p_ts->p_buffer[3] = ( b_adaptation_field ? 0x30 : 0x10 ) |
        p_stream->i_continuity_counter;
    p_stream->i_continuity_counter = (p_stream->i_continuity_counter+1)%16;
    p_stream->b_discontinuity = p_pes->i_flags & BLOCK_FLAG_DISCONTINUITY;
    if( b_adaptation_field )
    {
        int i_stuffing = i_payload_max - i_payload;
        if( b_pcr )
        {
            p_ts->i_flags |= BLOCK_FLAG_CLOCK;
            p_ts->p_buffer[4] = 7 + i_stuffing;
            p_ts->p_buffer[5] = 0x10;   /* flags */
            if( p_stream->b_discontinuity )
            {
                p_ts->p_buffer[5] |= 0x80; /* flag TS dicontinuity */
                p_stream->b_discontinuity = false;
            }
            p_ts->p_buffer[6] = 0 &0xff;
            p_ts->p_buffer[7] = 0 &0xff;
            p_ts->p_buffer[8] = 0 &0xff;
            p_ts->p_buffer[9] = 0 &0xff;
            p_ts->p_buffer[10]= ( 0 &0x80 ) | 0x7e;
            p_ts->p_buffer[11]= 0;
            for (int i = 12; i < 12 + i_stuffing; i++ )
            {
                p_ts->p_buffer[i] = 0xff;
            }
        }
        else
        {
            p_ts->p_buffer[4] = i_stuffing - 1;
            if( i_stuffing > 1 )
            {
                p_ts->p_buffer[5] = 0x00;
                for (int i = 6; i < 6 + i_stuffing - 2; i++ )
                {
                    p_ts->p_buffer[i] = 0xff;
                }
            }
        }
    }
    /* copy payload */
    memcpy( &p_ts->p_buffer[188 - i_payload],
            &p_pes->p_buffer[p_stream->i_pes_used], i_payload );
    p_stream->i_pes_used += i_payload;
    p_stream->i_pes_dts = p_pes->i_dts + p_pes->i_length *
        p_stream->i_pes_used / p_pes->i_buffer;
    p_stream->i_pes_length -= p_pes->i_length * i_payload / p_pes->i_buffer;
    if( p_stream->i_pes_used >= (int)p_pes->i_buffer )
    {
        block_Release(BufferChainGet( &p_stream->chain_pes ));
        p_pes = p_stream->chain_pes.p_first;
        p_stream->i_pes_length = 0;
        if( p_pes )
        {
            p_stream->i_pes_dts = p_pes->i_dts;
            while( p_pes )
            {
                p_stream->i_pes_length += p_pes->i_length;
                p_pes = p_pes->p_next;
            }
        }
        else
        {
            p_stream->i_pes_dts = 0;
        }
        p_stream->i_pes_used = 0;
    }
    return p_ts;
}
