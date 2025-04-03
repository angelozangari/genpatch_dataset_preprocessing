}
static void* CommandThread( void *obj )
{
    vod_t *p_vod = (vod_t*)obj;
    vod_sys_t *p_sys = p_vod->p_sys;
    for( ;; )
    {
        block_t *p_block_cmd = block_FifoGet( p_sys->p_fifo_cmd );
        rtsp_cmd_t cmd;
        if( !p_block_cmd )
            break;
        int canc = vlc_savecancel ();
        memcpy( &cmd, p_block_cmd->p_buffer, sizeof(cmd) );
        block_Release( p_block_cmd );
        /* */
        switch( cmd.i_type )
        {
        case RTSP_CMD_TYPE_ADD:
            MediaSetup(p_vod, cmd.p_media, cmd.psz_arg);
            break;
        case RTSP_CMD_TYPE_DEL:
            MediaDel(p_vod, cmd.p_media);
            break;
        case RTSP_CMD_TYPE_STOP:
            vod_MediaControl( p_vod, cmd.p_media, cmd.psz_arg, VOD_MEDIA_STOP );
            break;
        default:
            break;
        }
        free( cmd.psz_arg );
        vlc_restorecancel (canc);
    }
    return NULL;
}
