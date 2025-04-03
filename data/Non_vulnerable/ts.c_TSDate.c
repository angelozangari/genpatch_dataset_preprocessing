}
static void TSDate( sout_mux_t *p_mux, sout_buffer_chain_t *p_chain_ts,
                    mtime_t i_pcr_length, mtime_t i_pcr_dts )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    int i_packet_count = p_chain_ts->i_depth;
    if ( i_pcr_length / 1000 > 0 )
    {
        int i_bitrate = ((uint64_t)i_packet_count * 188 * 8000)
                          / (uint64_t)(i_pcr_length / 1000);
        if ( p_sys->i_bitrate_max && p_sys->i_bitrate_max < i_bitrate )
        {
            msg_Warn( p_mux, "max bitrate exceeded at %"PRId64
                      " (%d bi/s for %d pkt in %"PRId64" us)",
                      i_pcr_dts + p_sys->i_shaping_delay * 3 / 2 - mdate(),
                      i_bitrate, i_packet_count, i_pcr_length);
        }
    }
    else
    {
        /* This shouldn't happen, but happens in some rare heavy load
         * and packet losses conditions. */
        i_pcr_length = i_packet_count;
    }
    /* msg_Dbg( p_mux, "real pck=%d", i_packet_count ); */
    for (int i = 0; i < i_packet_count; i++ )
    {
        block_t *p_ts = BufferChainGet( p_chain_ts );
        mtime_t i_new_dts = i_pcr_dts + i_pcr_length * i / i_packet_count;
        p_ts->i_dts    = i_new_dts;
        p_ts->i_length = i_pcr_length / i_packet_count;
        if( p_ts->i_flags & BLOCK_FLAG_CLOCK )
        {
            /* msg_Dbg( p_mux, "pcr=%lld ms", p_ts->i_dts / 1000 ); */
            TSSetPCR( p_ts, p_ts->i_dts - p_sys->i_dts_delay );
        }
        if( p_ts->i_flags & BLOCK_FLAG_SCRAMBLED )
        {
            vlc_mutex_lock( &p_sys->csa_lock );
            csa_Encrypt( p_sys->csa, p_ts->p_buffer, p_sys->i_csa_pkt_size );
            vlc_mutex_unlock( &p_sys->csa_lock );
        }
        /* latency */
        p_ts->i_dts += p_sys->i_shaping_delay * 3 / 2;
        sout_AccessOutWrite( p_mux->p_access, p_ts );
    }
}
