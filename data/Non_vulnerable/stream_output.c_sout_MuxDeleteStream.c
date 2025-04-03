 *****************************************************************************/
void sout_MuxDeleteStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    int i_index;
    if( p_mux->b_waiting_stream
     && block_FifoCount( p_input->p_fifo ) > 0 )
    {
        /* We stop waiting, and call the muxer for taking care of the data
         * before we remove this es */
        p_mux->b_waiting_stream = false;
        p_mux->pf_mux( p_mux );
    }
    TAB_FIND( p_mux->i_nb_inputs, p_mux->pp_inputs, p_input, i_index );
    if( i_index >= 0 )
    {
        if( p_mux->pf_delstream( p_mux, p_input ) < 0 )
        {
            msg_Err( p_mux, "cannot delete this stream from mux" );
        }
        /* remove the entry */
        TAB_REMOVE( p_mux->i_nb_inputs, p_mux->pp_inputs, p_input );
        if( p_mux->i_nb_inputs == 0 )
        {
            msg_Warn( p_mux, "no more input streams for this mux" );
        }
        block_FifoRelease( p_input->p_fifo );
        free( p_input );
    }
}
