}
static void MediaAskDel ( vod_t *p_vod, vod_media_t *p_media )
{
    msg_Dbg( p_vod, "deleting media" );
    CommandPush( p_vod, RTSP_CMD_TYPE_DEL, p_media, NULL );
}
