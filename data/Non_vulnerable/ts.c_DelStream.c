 *****************************************************************************/
static int DelStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    ts_stream_t     *p_stream = (ts_stream_t*)p_input->p_sys;
    int              pid;
    msg_Dbg( p_mux, "removing input pid=%d", p_stream->i_pid );
    if( p_sys->i_pcr_pid == p_stream->i_pid )
    {
        /* Find a new pcr stream (Prefer Video Stream) */
        p_sys->i_pcr_pid = 0x1fff;
        p_sys->p_pcr_input = NULL;
        for (int i = 0; i < p_mux->i_nb_inputs; i++ )
        {
            if( p_mux->pp_inputs[i] == p_input )
            {
                continue;
            }
            if( p_mux->pp_inputs[i]->p_fmt->i_cat == VIDEO_ES )
            {
                p_sys->i_pcr_pid  =
                    ((ts_stream_t*)p_mux->pp_inputs[i]->p_sys)->i_pid;
                p_sys->p_pcr_input= p_mux->pp_inputs[i];
                break;
            }
            else if( p_mux->pp_inputs[i]->p_fmt->i_cat != SPU_ES &&
                     p_sys->i_pcr_pid == 0x1fff )
            {
                p_sys->i_pcr_pid  =
                    ((ts_stream_t*)p_mux->pp_inputs[i]->p_sys)->i_pid;
                p_sys->p_pcr_input= p_mux->pp_inputs[i];
            }
        }
        if( p_sys->p_pcr_input )
        {
            /* Empty TS buffer */
            /* FIXME */
        }
        msg_Dbg( p_mux, "new PCR PID is %d", p_sys->i_pcr_pid );
    }
    /* Empty all data in chain_pes */
    BufferChainClean( &p_stream->chain_pes );
    free(p_stream->lang);
    free( p_stream->p_extra );
    if( p_stream->i_stream_id == 0xfa ||
        p_stream->i_stream_id == 0xfb ||
        p_stream->i_stream_id == 0xfe )
    {
        p_sys->i_mpeg4_streams--;
    }
    pid = var_GetInteger( p_mux, SOUT_CFG_PREFIX "pid-video" );
    if ( pid > 0 && pid == p_stream->i_pid )
    {
        p_sys->i_pid_video = pid;
        msg_Dbg( p_mux, "freeing video PID %d", pid);
    }
    pid = var_GetInteger( p_mux, SOUT_CFG_PREFIX "pid-audio" );
    if ( pid > 0 && pid == p_stream->i_pid )
    {
        p_sys->i_pid_audio = pid;
        msg_Dbg( p_mux, "freeing audio PID %d", pid);
    }
    pid = var_GetInteger( p_mux, SOUT_CFG_PREFIX "pid-spu" );
    if ( pid > 0 && pid == p_stream->i_pid )
    {
        p_sys->i_pid_spu = pid;
        msg_Dbg( p_mux, "freeing spu PID %d", pid);
    }
    free( p_stream );
    /* We only change PMT version (PAT isn't changed) */
    p_sys->i_pmt_version_number++;
    p_sys->i_pmt_version_number %= 32;
    return VLC_SUCCESS;
}
