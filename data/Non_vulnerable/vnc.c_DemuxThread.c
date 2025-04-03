 *****************************************************************************/
static void *DemuxThread( void *p_data )
{
    demux_t *p_demux = (demux_t *) p_data;
    demux_sys_t  *p_sys = p_demux->p_sys;
    mtime_t i_next_frame_date = mdate() + p_sys->i_frame_interval;
    int i_status;
    for(;;)
    {
        p_sys->i_cancel_state = vlc_savecancel();
        i_status = WaitForMessage( p_sys->p_client, p_sys->i_frame_interval );
        vlc_restorecancel( p_sys->i_cancel_state );
        /* Ensure we're not building frames too fast */
        /* as WaitForMessage takes only a maximum wait */
        mwait( i_next_frame_date );
        i_next_frame_date += p_sys->i_frame_interval;
        if ( i_status > 0 )
        {
            p_sys->p_client->frameBuffer = p_sys->p_block->p_buffer;
            p_sys->i_cancel_state = vlc_savecancel();
            i_status = HandleRFBServerMessage( p_sys->p_client );
            vlc_restorecancel( p_sys->i_cancel_state );
            if ( ! i_status )
            {
                msg_Warn( p_demux, "Cannot get announced data. Server closed ?" );
                es_out_Del( p_demux->out, p_sys->es );
                p_sys->es = NULL;
                return NULL;
            }
            else
            {
                block_t *p_block = block_Duplicate( p_sys->p_block );
                if ( p_block ) /* drop frame/content if no next block */
                {
                    p_sys->p_block->i_dts = p_sys->p_block->i_pts = mdate();
                    es_out_Control( p_demux->out, ES_OUT_SET_PCR, p_sys->p_block->i_pts );
                    es_out_Send( p_demux->out, p_sys->es, p_sys->p_block );
                    p_sys->p_block = p_block;
                }
            }
        }
    }
    return NULL;
}
