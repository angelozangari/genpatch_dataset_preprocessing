/* */
static int net_ListenSingle (vlc_object_t *obj, const char *host, int port,
                             int protocol)
{
    struct addrinfo hints = {
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = protocol,
        .ai_flags = AI_PASSIVE | AI_NUMERICSERV | AI_IDN,
    }, *res;
    if (host && !*host)
        host = NULL;
    msg_Dbg (obj, "net: opening %s datagram port %d",
             host ? host : "any", port);
    int val = vlc_getaddrinfo (host, port, &hints, &res);
    if (val)
    {
        msg_Err (obj, "Cannot resolve %s port %d : %s", host, port,
                 gai_strerror (val));
        return -1;
    }
    val = -1;
    for (const struct addrinfo *ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        int fd = net_Socket (obj, ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);
        if (fd == -1)
        {
            msg_Dbg (obj, "socket error: %s", vlc_strerror_c(net_errno));
            continue;
        }
#ifdef IPV6_V6ONLY
        /* Try dual-mode IPv6 if available. */
        if (ptr->ai_family == AF_INET6)
            setsockopt (fd, SOL_IPV6, IPV6_V6ONLY, &(int){ 0 }, sizeof (int));
#endif
        fd = net_SetupDgramSocket( obj, fd, ptr );
        if( fd == -1 )
            continue;
        if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen)
         && net_Subscribe (obj, fd, ptr->ai_addr, ptr->ai_addrlen))
        {
            net_Close (fd);
            continue;
        }
        val = fd;
        break;
    }
    freeaddrinfo (res);
    return val;
}
