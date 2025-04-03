}
static void CommandPush( vod_t *p_vod, rtsp_cmd_type_t i_type,
                         vod_media_t *p_media, const char *psz_arg )
{
    rtsp_cmd_t cmd;
    block_t *p_cmd;
    cmd.i_type = i_type;
    cmd.p_media = p_media;
    if( psz_arg )
        cmd.psz_arg = strdup(psz_arg);
    else
        cmd.psz_arg = NULL;
    p_cmd = block_Alloc( sizeof(rtsp_cmd_t) );
    memcpy( p_cmd->p_buffer, &cmd, sizeof(cmd) );
    block_FifoPut( p_vod->p_sys->p_fifo_cmd, p_cmd );
}
