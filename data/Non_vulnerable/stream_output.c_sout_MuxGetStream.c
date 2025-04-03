 *****************************************************************************/
int sout_MuxGetStream( sout_mux_t *p_mux, int i_blocks, mtime_t *pi_dts )
{
    mtime_t i_dts = 0;
    int     i_stream = -1;
    for( int i = 0; i < p_mux->i_nb_inputs; i++ )
    {
        sout_input_t *p_input = p_mux->pp_inputs[i];
        block_t *p_data;
        if( block_FifoCount( p_input->p_fifo ) < i_blocks )
            continue;
        p_data = block_FifoShow( p_input->p_fifo );
        if( i_stream < 0 || p_data->i_dts < i_dts )
        {
            i_stream = i;
            i_dts    = p_data->i_dts;
        }
    }
    if( pi_dts ) *pi_dts = i_dts;
    return i_stream;
}
