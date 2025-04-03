 *****************************************************************************/
sout_mux_t * sout_MuxNew( sout_instance_t *p_sout, const char *psz_mux,
                          sout_access_out_t *p_access )
{
    sout_mux_t *p_mux;
    char       *psz_next;
    p_mux = vlc_custom_create( p_sout, sizeof( *p_mux ), "mux" );
    if( p_mux == NULL )
        return NULL;
    p_mux->p_sout = p_sout;
    psz_next = config_ChainCreate( &p_mux->psz_mux, &p_mux->p_cfg, psz_mux );
    free( psz_next );
    p_mux->p_access     = p_access;
    p_mux->pf_control   = NULL;
    p_mux->pf_addstream = NULL;
    p_mux->pf_delstream = NULL;
    p_mux->pf_mux       = NULL;
    p_mux->i_nb_inputs  = 0;
    p_mux->pp_inputs    = NULL;
    p_mux->p_sys        = NULL;
    p_mux->p_module     = NULL;
    p_mux->b_add_stream_any_time = false;
    p_mux->b_waiting_stream = true;
    p_mux->i_add_stream_start = -1;
    p_mux->p_module =
        module_need( p_mux, "sout mux", p_mux->psz_mux, true );
    if( p_mux->p_module == NULL )
    {
        FREENULL( p_mux->psz_mux );
        vlc_object_release( p_mux );
        return NULL;
    }
    /* *** probe mux capacity *** */
    if( p_mux->pf_control )
    {
        int b_answer = false;
        if( sout_MuxControl( p_mux, MUX_CAN_ADD_STREAM_WHILE_MUXING,
                             &b_answer ) )
        {
            b_answer = false;
        }
        if( b_answer )
        {
            msg_Dbg( p_sout, "muxer support adding stream at any time" );
            p_mux->b_add_stream_any_time = true;
            p_mux->b_waiting_stream = false;
            /* If we control the output pace then it's better to wait before
             * starting muxing (generates better streams/files). */
            if( !p_sout->i_out_pace_nocontrol )
            {
                b_answer = true;
            }
            else if( sout_MuxControl( p_mux, MUX_GET_ADD_STREAM_WAIT,
                                      &b_answer ) )
            {
                b_answer = false;
            }
            if( b_answer )
            {
                msg_Dbg( p_sout, "muxer prefers to wait for all ES before "
                         "starting to mux" );
                p_mux->b_waiting_stream = true;
            }
        }
    }
    return p_mux;
}
