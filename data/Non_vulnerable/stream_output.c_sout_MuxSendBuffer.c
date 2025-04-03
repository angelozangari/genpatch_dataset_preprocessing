 *****************************************************************************/
int sout_MuxSendBuffer( sout_mux_t *p_mux, sout_input_t *p_input,
                         block_t *p_buffer )
{
    mtime_t i_dts = p_buffer->i_dts;
    block_FifoPut( p_input->p_fifo, p_buffer );
    if( p_mux->p_sout->i_out_pace_nocontrol )
    {
        mtime_t current_date = mdate();
        if ( current_date > i_dts )
            msg_Warn( p_mux, "late buffer for mux input (%"PRId64")",
                      current_date - i_dts );
    }
    if( p_mux->b_waiting_stream )
    {
        const int64_t i_caching = var_GetInteger( p_mux->p_sout, "sout-mux-caching" ) * INT64_C(1000);
        if( p_mux->i_add_stream_start < 0 )
            p_mux->i_add_stream_start = i_dts;
        /* Wait until we have enought data before muxing */
        if( p_mux->i_add_stream_start < 0 ||
            i_dts < p_mux->i_add_stream_start + i_caching )
            return VLC_SUCCESS;
        p_mux->b_waiting_stream = false;
    }
    return p_mux->pf_mux( p_mux );
}
