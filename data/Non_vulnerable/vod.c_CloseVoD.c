 *****************************************************************************/
void CloseVoD( vlc_object_t * p_this )
{
    vod_t *p_vod = (vod_t *)p_this;
    vod_sys_t *p_sys = p_vod->p_sys;
    /* Stop command thread */
    vlc_cancel( p_sys->thread );
    vlc_join( p_sys->thread, NULL );
    while( block_FifoCount( p_sys->p_fifo_cmd ) > 0 )
    {
        rtsp_cmd_t cmd;
        block_t *p_block_cmd = block_FifoGet( p_sys->p_fifo_cmd );
        memcpy( &cmd, p_block_cmd->p_buffer, sizeof(cmd) );
        block_Release( p_block_cmd );
        if ( cmd.i_type == RTSP_CMD_TYPE_DEL )
            MediaDel(p_vod, cmd.p_media);
        free( cmd.psz_arg );
    }
    block_FifoRelease( p_sys->p_fifo_cmd );
    free( p_sys->psz_rtsp_path );
    free( p_sys );
}
