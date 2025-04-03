 *****************************************************************************/
int net_Connect( vlc_object_t *p_this, const char *psz_host, int i_port,
                 int type, int proto )
{
    const char      *psz_realhost;
    char            *psz_socks;
    int             i_realport, i_handle = -1;
    int evfd = vlc_object_waitpipe (p_this);
    if (evfd == -1)
        return -1;
    psz_socks = var_InheritString( p_this, "socks" );
    if( psz_socks != NULL )
    {
        char *psz = strchr( psz_socks, ':' );
        if( psz )
            *psz++ = '\0';
        psz_realhost = psz_socks;
        i_realport = ( psz != NULL ) ? atoi( psz ) : 1080;
        msg_Dbg( p_this, "net: connecting to %s port %d (SOCKS) "
                 "for %s port %d", psz_realhost, i_realport,
                 psz_host, i_port );
        /* We only implement TCP with SOCKS */
        switch( type )
        {
            case 0:
                type = SOCK_STREAM;
            case SOCK_STREAM:
                break;
            default:
                msg_Err( p_this, "Socket type not supported through SOCKS" );
                free( psz_socks );
                return -1;
        }
        switch( proto )
        {
            case 0:
                proto = IPPROTO_TCP;
            case IPPROTO_TCP:
                break;
            default:
                msg_Err( p_this, "Transport not supported through SOCKS" );
                free( psz_socks );
                return -1;
        }
    }
    else
    {
        psz_realhost = psz_host;
        i_realport = i_port;
        msg_Dbg( p_this, "net: connecting to %s port %d", psz_realhost,
                 i_realport );
    }
    struct addrinfo hints = {
        .ai_socktype = type,
        .ai_protocol = proto,
        .ai_flags = AI_NUMERICSERV | AI_IDN,
    }, *res;
    int val = vlc_getaddrinfo (psz_realhost, i_realport, &hints, &res);
    if (val)
    {
        msg_Err (p_this, "cannot resolve %s port %d : %s", psz_realhost,
                 i_realport, gai_strerror (val));
        free( psz_socks );
        return -1;
    }
    free( psz_socks );
    int timeout = var_InheritInteger (p_this, "ipv4-timeout");
    if (timeout < 0)
        timeout = -1;
    for (struct addrinfo *ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        int fd = net_Socket( p_this, ptr->ai_family,
                             ptr->ai_socktype, ptr->ai_protocol );
        if( fd == -1 )
        {
            msg_Dbg( p_this, "socket error: %s", vlc_strerror_c(net_errno) );
            continue;
        }
        if( connect( fd, ptr->ai_addr, ptr->ai_addrlen ) )
        {
            int val;
            if( net_errno != EINPROGRESS && net_errno != EINTR )
            {
                msg_Err( p_this, "connection failed: %s",
                         vlc_strerror_c(net_errno) );
                goto next_ai;
            }
            struct pollfd ufd[2] = {
                { .fd = fd,   .events = POLLOUT },
                { .fd = evfd, .events = POLLIN },
            };
            do
                /* NOTE: timeout screwed up if we catch a signal (EINTR) */
                val = poll (ufd, sizeof (ufd) / sizeof (ufd[0]), timeout);
            while ((val == -1) && (net_errno == EINTR));
            switch (val)
            {
                 case -1: /* error */
                     msg_Err (p_this, "polling error: %s",
                              vlc_strerror_c(net_errno));
                     goto next_ai;
                 case 0: /* timeout */
                     msg_Warn (p_this, "connection timed out");
                     goto next_ai;
                 default: /* something happended */
                     if (ufd[1].revents)
                         goto next_ai; /* LibVLC object killed */
            }
            /* There is NO WAY around checking SO_ERROR.
             * Don't ifdef it out!!! */
            if (getsockopt (fd, SOL_SOCKET, SO_ERROR, &val,
                            &(socklen_t){ sizeof (val) }) || val)
            {
                msg_Err (p_this, "connection failed: %s",
                         vlc_strerror_c(val));
                goto next_ai;
            }
        }
        msg_Dbg( p_this, "connection succeeded (socket = %d)", fd );
        i_handle = fd; /* success! */
        break;
next_ai: /* failure */
        net_Close( fd );
        continue;
    }
    freeaddrinfo( res );
    if( i_handle == -1 )
        return -1;
    if( psz_socks != NULL )
    {
        /* NOTE: psz_socks already free'd! */
        char *psz_user = var_InheritString( p_this, "socks-user" );
        char *psz_pwd  = var_InheritString( p_this, "socks-pwd" );
        if( SocksHandshakeTCP( p_this, i_handle, 5, psz_user, psz_pwd,
                               psz_host, i_port ) )
        {
            msg_Err( p_this, "SOCKS handshake failed" );
            net_Close( i_handle );
            i_handle = -1;
        }
        free( psz_user );
        free( psz_pwd );
    }
    return i_handle;
}
