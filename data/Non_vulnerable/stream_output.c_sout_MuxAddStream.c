 *****************************************************************************/
sout_input_t *sout_MuxAddStream( sout_mux_t *p_mux, es_format_t *p_fmt )
{
    sout_input_t *p_input;
    if( !p_mux->b_add_stream_any_time && !p_mux->b_waiting_stream )
    {
        msg_Err( p_mux, "cannot add a new stream (unsupported while muxing "
                        "to this format). You can try increasing sout-mux-caching value" );
        return NULL;
    }
    msg_Dbg( p_mux, "adding a new input" );
    /* create a new sout input */
    p_input = malloc( sizeof( sout_input_t ) );
    if( !p_input )
        return NULL;
    p_input->p_fmt  = p_fmt;
    p_input->p_fifo = block_FifoNew();
    p_input->p_sys  = NULL;
    TAB_APPEND( p_mux->i_nb_inputs, p_mux->pp_inputs, p_input );
    if( p_mux->pf_addstream( p_mux, p_input ) < 0 )
    {
        msg_Err( p_mux, "cannot add this stream" );
        TAB_REMOVE( p_mux->i_nb_inputs, p_mux->pp_inputs, p_input );
        block_FifoRelease( p_input->p_fifo );
        free( p_input );
        return NULL;
    }
    return p_input;
}
