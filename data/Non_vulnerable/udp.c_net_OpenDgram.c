 *****************************************************************************/
int net_OpenDgram( vlc_object_t *obj, const char *psz_bind, int i_bind,
                   const char *psz_server, int i_server, int protocol )
{
    if ((psz_server == NULL) || (psz_server[0] == '\0'))
        return net_ListenSingle (obj, psz_bind, i_bind, protocol);
    msg_Dbg (obj, "net: connecting to [%s]:%d from [%s]:%d",
             psz_server, i_server, psz_bind, i_bind);
    struct addrinfo hints = {
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = protocol,
        .ai_flags = AI_NUMERICSERV | AI_IDN,
    }, *loc, *rem;
    int val = vlc_getaddrinfo (psz_server, i_server, &hints, &rem);
    if (val)
    {
        msg_Err (obj, "cannot resolve %s port %d : %s", psz_bind, i_bind,
                 gai_strerror (val));
        return -1;
    }
    hints.ai_flags |= AI_PASSIVE;
    val = vlc_getaddrinfo (psz_bind, i_bind, &hints, &loc);
    if (val)
    {
        msg_Err (obj, "cannot resolve %s port %d : %s", psz_bind, i_bind,
                 gai_strerror (val));
        freeaddrinfo (rem);
        return -1;
    }
    val = -1;
    for (struct addrinfo *ptr = loc; ptr != NULL; ptr = ptr->ai_next)
    {
        int fd = net_Socket (obj, ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);
        if (fd == -1)
            continue; // usually, address family not supported
        fd = net_SetupDgramSocket( obj, fd, ptr );
        if( fd == -1 )
            continue;
        for (struct addrinfo *ptr2 = rem; ptr2 != NULL; ptr2 = ptr2->ai_next)
        {
            if ((ptr2->ai_family != ptr->ai_family)
             || (ptr2->ai_socktype != ptr->ai_socktype)
             || (ptr2->ai_protocol != ptr->ai_protocol))
                continue;
            if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen)
              ? net_SourceSubscribe (obj, fd,
                                     ptr2->ai_addr, ptr2->ai_addrlen,
                                     ptr->ai_addr, ptr->ai_addrlen)
              : connect (fd, ptr2->ai_addr, ptr2->ai_addrlen))
            {
                msg_Err (obj, "cannot connect to %s port %d: %s",
                         psz_server, i_server, vlc_strerror_c(net_errno));
                continue;
            }
            val = fd;
            break;
        }
        if (val != -1)
            break;
        net_Close (fd);
    }
    freeaddrinfo (rem);
    freeaddrinfo (loc);
    return val;
}
