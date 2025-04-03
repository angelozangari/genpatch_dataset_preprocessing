 *****************************************************************************/
static int Mux( sout_mux_t *p_mux )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    if( p_sys->i_pcr_pid == 0x1fff )
    {
        for (int i = 0; i < p_mux->i_nb_inputs; i++ )
        {
            block_FifoEmpty( p_mux->pp_inputs[i]->p_fifo );
        }
        msg_Dbg( p_mux, "waiting for PCR streams" );
        return VLC_SUCCESS;
    }
    while (!MuxStreams(p_mux))
        ;
    return VLC_SUCCESS;
}
