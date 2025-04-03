/* returns true if needs more data */
static bool MuxStreams(sout_mux_t *p_mux )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    ts_stream_t *p_pcr_stream = (ts_stream_t*)p_sys->p_pcr_input->p_sys;
    sout_buffer_chain_t chain_ts;
    mtime_t i_shaping_delay = p_pcr_stream->b_key_frame
        ? p_pcr_stream->i_pes_length
        : p_sys->i_shaping_delay;
    bool b_ok = true;
    /* Accumulate enough data in the pcr stream (>i_shaping_delay) */
    /* Accumulate enough data in all other stream ( >= length of pcr)*/
    for (int i = -1; !b_ok || i < p_mux->i_nb_inputs; i++ )
    {
        if (i == p_mux->i_nb_inputs)
        {
            /* get enough PES packet for all input */
            b_ok = true;
            i = -1;
        }
        sout_input_t *p_input;
        if( i == -1 )
            p_input = p_sys->p_pcr_input;
        else if( p_mux->pp_inputs[i]->p_sys == p_pcr_stream )
            continue;
        else
            p_input = p_mux->pp_inputs[i];
        ts_stream_t *p_stream = (ts_stream_t*)p_input->p_sys;
        if( ( p_stream != p_pcr_stream ||
              p_stream->i_pes_length >= i_shaping_delay ) &&
            p_stream->i_pes_dts + p_stream->i_pes_length >=
            p_pcr_stream->i_pes_dts + p_pcr_stream->i_pes_length )
            continue;
        /* Need more data */
        if( block_FifoCount( p_input->p_fifo ) <= 1 )
        {
            if( ( p_input->p_fmt->i_cat == AUDIO_ES ) ||
                ( p_input->p_fmt->i_cat == VIDEO_ES ) )
            {
                /* We need more data */
                return true;
            }
            else if( block_FifoCount( p_input->p_fifo ) <= 0 )
            {
                /* spu, only one packet is needed */
                continue;
            }
            else if( p_input->p_fmt->i_cat == SPU_ES )
            {
                /* Don't mux the SPU yet if it is too early */
                block_t *p_spu = block_FifoShow( p_input->p_fifo );
                int64_t i_spu_delay = p_spu->i_dts - p_sys->first_dts - p_pcr_stream->i_pes_dts;
                if( ( i_spu_delay > i_shaping_delay ) &&
                    ( i_spu_delay < INT64_C(100000000) ) )
                    continue;
                if ( ( i_spu_delay >= INT64_C(100000000) ) ||
                     ( i_spu_delay < INT64_C(10000) ) )
                {
                    BufferChainClean( &p_stream->chain_pes );
                    p_stream->i_pes_dts = 0;
                    p_stream->i_pes_used = 0;
                    p_stream->i_pes_length = 0;
                    continue;
                }
            }
        }
        b_ok = false;
        block_t *p_data;
        if( p_stream == p_pcr_stream || p_sys->b_data_alignment
             || ((p_input->p_fmt->i_codec != VLC_CODEC_MPGA ) &&
                 (p_input->p_fmt->i_codec != VLC_CODEC_MP3) ) )
        {
            p_data = block_FifoGet( p_input->p_fifo );
            if (p_data->i_pts <= VLC_TS_INVALID)
                p_data->i_pts = p_data->i_dts;
            if( p_input->p_fmt->i_codec == VLC_CODEC_MP4A )
                p_data = Add_ADTS( p_data, p_input->p_fmt );
        }
        else
            p_data = FixPES( p_mux, p_input->p_fifo );
        if( block_FifoCount( p_input->p_fifo ) > 0 &&
            p_input->p_fmt->i_cat != SPU_ES )
        {
            block_t *p_next = block_FifoShow( p_input->p_fifo );
            p_data->i_length = p_next->i_dts - p_data->i_dts;
        }
        else if( p_input->p_fmt->i_codec !=
                   VLC_CODEC_SUBT )
            p_data->i_length = 1000;
        if (p_sys->first_dts == 0)
            p_sys->first_dts = p_data->i_dts;
        p_data->i_dts -= p_sys->first_dts;
        p_data->i_pts -= p_sys->first_dts;
        if( ( p_pcr_stream->i_pes_dts > 0 &&
              p_data->i_dts - 10000000 > p_pcr_stream->i_pes_dts +
              p_pcr_stream->i_pes_length ) ||
            p_data->i_dts < p_stream->i_pes_dts ||
            ( p_stream->i_pes_dts > 0 &&
              p_input->p_fmt->i_cat != SPU_ES &&
              p_data->i_dts - 10000000 > p_stream->i_pes_dts +
              p_stream->i_pes_length ) )
        {
            msg_Warn( p_mux, "packet with too strange dts "
                      "(dts=%"PRId64",old=%"PRId64",pcr=%"PRId64")",
                      p_data->i_dts, p_stream->i_pes_dts,
                      p_pcr_stream->i_pes_dts );
            block_Release( p_data );
            BufferChainClean( &p_stream->chain_pes );
            p_stream->i_pes_dts = 0;
            p_stream->i_pes_used = 0;
            p_stream->i_pes_length = 0;
            if( p_input->p_fmt->i_cat != SPU_ES )
            {
                BufferChainClean( &p_pcr_stream->chain_pes );
                p_pcr_stream->i_pes_dts = 0;
                p_pcr_stream->i_pes_used = 0;
                p_pcr_stream->i_pes_length = 0;
            }
            continue;
        }
        int i_header_size = 0;
        int i_max_pes_size = 0;
        int b_data_alignment = 0;
        if( p_input->p_fmt->i_cat == SPU_ES ) switch (p_input->p_fmt->i_codec)
        {
        case VLC_CODEC_SUBT:
            /* Prepend header */
            p_data = block_Realloc( p_data, 2, p_data->i_buffer );
            p_data->p_buffer[0] = ( (p_data->i_buffer - 2) >> 8) & 0xff;
            p_data->p_buffer[1] = ( (p_data->i_buffer - 2)     ) & 0xff;
            /* remove trailling \0 if any */
            if( p_data->i_buffer > 2 && !p_data->p_buffer[p_data->i_buffer-1] )
                p_data->i_buffer--;
            /* Append a empty sub (sub text only) */
            if( p_data->i_length > 0 &&
                ( p_data->i_buffer != 1 || *p_data->p_buffer != ' ' ) )
            {
                block_t *p_spu = block_Alloc( 3 );
                p_spu->i_dts = p_data->i_dts + p_data->i_length;
                p_spu->i_pts = p_spu->i_dts;
                p_spu->i_length = 1000;
                p_spu->p_buffer[0] = 0;
                p_spu->p_buffer[1] = 1;
                p_spu->p_buffer[2] = ' ';
                EStoPES( &p_spu, p_spu, p_input->p_fmt,
                             p_stream->i_stream_id, 1, 0, 0, 0 );
                p_data->p_next = p_spu;
            }
            break;
        case VLC_CODEC_TELETEXT:
            /* EN 300 472 */
            i_header_size = 0x24;
            b_data_alignment = 1;
            break;
        case VLC_CODEC_DVBS:
            /* EN 300 743 */
            b_data_alignment = 1;
            break;
        }
        else if( p_data->i_length < 0 || p_data->i_length > 2000000 )
        {
            /* FIXME choose a better value, but anyway we
             * should never have to do that */
            p_data->i_length = 1000;
        }
        p_stream->i_pes_length += p_data->i_length;
        if( p_stream->i_pes_dts == 0 )
        {
            p_stream->i_pes_dts = p_data->i_dts;
        }
        /* Convert to pes */
        if( p_stream->i_stream_id == 0xa0 && p_data->i_pts <= 0 )
        {
            /* XXX yes I know, it's awful, but it's needed,
             * so don't remove it ... */
            p_data->i_pts = p_data->i_dts;
        }
        if( p_input->p_fmt->i_codec == VLC_CODEC_DIRAC )
        {
            b_data_alignment = 1;
            /* dirac pes packets should be unbounded in
             * length, specify a suitibly large max size */
            i_max_pes_size = INT_MAX;
        }
         EStoPES ( &p_data, p_data, p_input->p_fmt, p_stream->i_stream_id,
                       1, b_data_alignment, i_header_size,
                       i_max_pes_size );
        BufferChainAppend( &p_stream->chain_pes, p_data );
        if( p_sys->b_use_key_frames && p_stream == p_pcr_stream
            && (p_data->i_flags & BLOCK_FLAG_TYPE_I)
            && !(p_data->i_flags & BLOCK_FLAG_NO_KEYFRAME)
            && (p_stream->i_pes_length > 400000) )
        {
            i_shaping_delay = p_stream->i_pes_length;
            p_stream->b_key_frame = 1;
        }
    }
    /* save */
    const mtime_t i_pcr_length = p_pcr_stream->i_pes_length;
    p_pcr_stream->b_key_frame = 0;
    /* msg_Dbg( p_mux, "starting muxing %lldms", i_pcr_length / 1000 ); */
    /* 2: calculate non accurate total size of muxed ts */
    int i_packet_count = 0;
    for (int i = 0; i < p_mux->i_nb_inputs; i++ )
    {
        ts_stream_t *p_stream = (ts_stream_t*)p_mux->pp_inputs[i]->p_sys;
        /* False for pcr stream but it will be enough to do PCR algo */
        for (block_t *p_pes = p_stream->chain_pes.p_first; p_pes != NULL;
             p_pes = p_pes->p_next )
        {
            int i_size = p_pes->i_buffer;
            if( p_pes->i_dts + p_pes->i_length >
                p_pcr_stream->i_pes_dts + p_pcr_stream->i_pes_length )
            {
                mtime_t i_frag = p_pcr_stream->i_pes_dts +
                    p_pcr_stream->i_pes_length - p_pes->i_dts;
                if( i_frag < 0 )
                {
                    /* Next stream */
                    break;
                }
                i_size = p_pes->i_buffer * i_frag / p_pes->i_length;
            }
            i_packet_count += ( i_size + 183 ) / 184;
        }
    }
    /* add overhead for PCR (not really exact) */
    i_packet_count += (8 * i_pcr_length / p_sys->i_pcr_delay + 175) / 176;
    /* 3: mux PES into TS */
    BufferChainInit( &chain_ts );
    /* append PAT/PMT  -> FIXME with big pcr delay it won't have enough pat/pmt */
    bool pat_was_previous = true; //This is to prevent unnecessary double PAT/PMT insertions
    GetPAT( p_mux, &chain_ts );
    GetPMT( p_mux, &chain_ts );
    int i_packet_pos = 0;
    i_packet_count += chain_ts.i_depth;
    /* msg_Dbg( p_mux, "estimated pck=%d", i_packet_count ); */
    const mtime_t i_pcr_dts = p_pcr_stream->i_pes_dts;
    for (;;)
    {
        int          i_stream = -1;
        mtime_t      i_dts = 0;
        ts_stream_t  *p_stream;
        /* Select stream (lowest dts) */
        for (int i = 0; i < p_mux->i_nb_inputs; i++ )
        {
            p_stream = (ts_stream_t*)p_mux->pp_inputs[i]->p_sys;
            if( p_stream->i_pes_dts == 0 )
            {
                continue;
            }
            if( i_stream == -1 || p_stream->i_pes_dts < i_dts )
            {
                i_stream = i;
                i_dts = p_stream->i_pes_dts;
            }
        }
        if( i_stream == -1 || i_dts > i_pcr_dts + i_pcr_length )
        {
            break;
        }
        p_stream = (ts_stream_t*)p_mux->pp_inputs[i_stream]->p_sys;
        sout_input_t *p_input = p_mux->pp_inputs[i_stream];
        /* do we need to issue pcr */
        bool b_pcr = false;
        if( p_stream == p_pcr_stream &&
            i_pcr_dts + i_packet_pos * i_pcr_length / i_packet_count >=
            p_sys->i_pcr + p_sys->i_pcr_delay )
        {
            b_pcr = true;
            p_sys->i_pcr = i_pcr_dts + i_packet_pos *
                i_pcr_length / i_packet_count;
        }
        /* Build the TS packet */
        block_t *p_ts = TSNew( p_mux, p_stream, b_pcr );
        if( p_sys->csa != NULL &&
             (p_input->p_fmt->i_cat != AUDIO_ES || p_sys->b_crypt_audio) &&
             (p_input->p_fmt->i_cat != VIDEO_ES || p_sys->b_crypt_video) )
        {
            p_ts->i_flags |= BLOCK_FLAG_SCRAMBLED;
        }
        i_packet_pos++;
        /* Write PAT/PMT before every keyframe if use-key-frames is enabled,
         * this helps to do segmenting with livehttp-output so it can cut segment
         * and start new one with pat,pmt,keyframe*/
        if( ( p_sys->b_use_key_frames ) && ( p_ts->i_flags & BLOCK_FLAG_TYPE_I ) )
        {
            if( likely( !pat_was_previous ) )
            {
                int startcount = chain_ts.i_depth;
                GetPAT( p_mux, &chain_ts );
                GetPMT( p_mux, &chain_ts );
                SetHeader( &chain_ts, startcount );
                i_packet_count += (chain_ts.i_depth - startcount );
            } else {
                SetHeader( &chain_ts, 0); //We just inserted pat/pmt,so just flag it instead of adding new one
            }
        }
        pat_was_previous = false;
        /* */
        BufferChainAppend( &chain_ts, p_ts );
    }
    /* 4: date and send */
    TSSchedule( p_mux, &chain_ts, i_pcr_length, i_pcr_dts );
    return false;
}
