}
static void TSSchedule( sout_mux_t *p_mux, sout_buffer_chain_t *p_chain_ts,
                        mtime_t i_pcr_length, mtime_t i_pcr_dts )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    sout_buffer_chain_t new_chain;
    int i_packet_count = p_chain_ts->i_depth;
    BufferChainInit( &new_chain );
    if ( i_pcr_length <= 0 )
    {
        i_pcr_length = i_packet_count;
    }
    for (int i = 0; i < i_packet_count; i++ )
    {
        block_t *p_ts = BufferChainGet( p_chain_ts );
        mtime_t i_new_dts = i_pcr_dts + i_pcr_length * i / i_packet_count;
        BufferChainAppend( &new_chain, p_ts );
        if (!p_ts->i_dts || p_ts->i_dts + p_sys->i_dts_delay * 2/3 >= i_new_dts)
            continue;
        mtime_t i_max_diff = i_new_dts - p_ts->i_dts;
        mtime_t i_cut_dts = p_ts->i_dts;
        p_ts = BufferChainPeek( p_chain_ts );
        i++;
        i_new_dts = i_pcr_dts + i_pcr_length * i / i_packet_count;
        while ( p_ts != NULL && i_new_dts - p_ts->i_dts >= i_max_diff )
        {
            p_ts = BufferChainGet( p_chain_ts );
            i_max_diff = i_new_dts - p_ts->i_dts;
            i_cut_dts = p_ts->i_dts;
            BufferChainAppend( &new_chain, p_ts );
            p_ts = BufferChainPeek( p_chain_ts );
            i++;
            i_new_dts = i_pcr_dts + i_pcr_length * i / i_packet_count;
        }
        msg_Dbg( p_mux, "adjusting rate at %"PRId64"/%"PRId64" (%d/%d)",
                 i_cut_dts - i_pcr_dts, i_pcr_length, new_chain.i_depth,
                 p_chain_ts->i_depth );
        if ( new_chain.i_depth )
            TSDate( p_mux, &new_chain, i_cut_dts - i_pcr_dts, i_pcr_dts );
        if ( p_chain_ts->i_depth )
            TSSchedule( p_mux, p_chain_ts, i_pcr_dts + i_pcr_length - i_cut_dts,
                        i_cut_dts );
        return;
    }
    if ( new_chain.i_depth )
        TSDate( p_mux, &new_chain, i_pcr_length, i_pcr_dts );
}
