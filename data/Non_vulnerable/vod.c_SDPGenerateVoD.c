 *****************************************************************************/
char *SDPGenerateVoD( const vod_media_t *p_media, const char *rtsp_url )
{
    char *psz_sdp;
    assert(rtsp_url != NULL);
    /* Check against URL format rtsp://[<ipv6>]:<port>/<path> */
    bool ipv6 = strlen( rtsp_url ) > 7 && rtsp_url[7] == '[';
    /* Dummy destination address for RTSP */
    struct sockaddr_storage dst;
    socklen_t dstlen = ipv6 ? sizeof( struct sockaddr_in6 )
                            : sizeof( struct sockaddr_in );
    memset (&dst, 0, dstlen);
    dst.ss_family = ipv6 ? AF_INET6 : AF_INET;
#ifdef HAVE_SA_LEN
    dst.ss_len = dstlen;
#endif
    psz_sdp = vlc_sdp_Start( VLC_OBJECT( p_media->p_vod ), "sout-rtp-",
                             NULL, 0, (struct sockaddr *)&dst, dstlen );
    if( psz_sdp == NULL )
        return NULL;
    if( p_media->i_length > 0 )
    {
        lldiv_t d = lldiv( p_media->i_length / 1000, 1000 );
        sdp_AddAttribute( &psz_sdp, "range"," npt=0-%lld.%03u", d.quot,
                          (unsigned)d.rem );
    }
    sdp_AddAttribute ( &psz_sdp, "control", "%s", rtsp_url );
    /* No locking needed, the ES table can't be modified now */
    for( int i = 0; i < p_media->i_es; i++ )
    {
        media_es_t *p_es = p_media->es[i];
        rtp_format_t *rtp_fmt = &p_es->rtp_fmt;
        const char *mime_major; /* major MIME type */
        switch( rtp_fmt->cat )
        {
            case VIDEO_ES:
                mime_major = "video";
                break;
            case AUDIO_ES:
                mime_major = "audio";
                break;
            case SPU_ES:
                mime_major = "text";
                break;
            default:
                continue;
        }
        sdp_AddMedia( &psz_sdp, mime_major, "RTP/AVP", 0,
                      rtp_fmt->payload_type, false, 0,
                      rtp_fmt->ptname, rtp_fmt->clock_rate, rtp_fmt->channels,
                      rtp_fmt->fmtp );
        char *track_url = RtspAppendTrackPath( p_es->rtsp_id, rtsp_url );
        if( track_url != NULL )
        {
            sdp_AddAttribute ( &psz_sdp, "control", "%s", track_url );
            free( track_url );
        }
    }
    return psz_sdp;
}
