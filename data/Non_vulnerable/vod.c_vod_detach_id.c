/* Remove references to the RTP id from its RTSP track */
void vod_detach_id(vod_media_t *p_media, const char *psz_session,
                   sout_stream_id_sys_t *sout_id)
{
    RtspTrackDetach(p_media->rtsp, psz_session, sout_id);
}
