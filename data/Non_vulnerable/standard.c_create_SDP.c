}
static void create_SDP(sout_stream_t *p_stream, sout_access_out_t *p_access)
{
    sout_stream_sys_t   *p_sys = p_stream->p_sys;
    static const struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = 0,
        .ai_flags = AI_NUMERICHOST | AI_NUMERICSERV | AI_IDN,
    };
    char *shost = var_GetNonEmptyString (p_access, "src-addr");
    char *dhost = var_GetNonEmptyString (p_access, "dst-addr");
    int sport = var_GetInteger (p_access, "src-port");
    int dport = var_GetInteger (p_access, "dst-port");
    struct sockaddr_storage src, dst;
    socklen_t srclen = 0, dstlen = 0;
    struct addrinfo *res;
    if (!vlc_getaddrinfo (dhost, dport, &hints, &res))
    {
        memcpy (&dst, res->ai_addr, dstlen = res->ai_addrlen);
        freeaddrinfo (res);
    }
    if (!vlc_getaddrinfo (shost, sport, &hints, &res))
    {
        memcpy (&src, res->ai_addr, srclen = res->ai_addrlen);
        freeaddrinfo (res);
    }
    char *head = vlc_sdp_Start (VLC_OBJECT (p_stream), SOUT_CFG_PREFIX,
            (struct sockaddr *)&src, srclen,
            (struct sockaddr *)&dst, dstlen);
    free (shost);
    if (head != NULL)
    {
        char *psz_sdp = NULL;
        if (asprintf (&psz_sdp, "%s"
                    "m=video %d udp mpeg\r\n", head, dport) == -1)
            psz_sdp = NULL;
        free (head);
        /* Register the SDP with the SAP thread */
        if (psz_sdp)
        {
            msg_Dbg (p_stream, "Generated SDP:\n%s", psz_sdp);
            p_sys->p_session =
                sout_AnnounceRegisterSDP (p_stream, psz_sdp, dhost);
            free( psz_sdp );
        }
    }
    free (dhost);
}
