 *****************************************************************************/
int net_ConnectDgram( vlc_object_t *p_this, const char *psz_host, int i_port,
                      int i_hlim, int proto )
{
    struct addrinfo hints = {
        .ai_socktype = SOCK_DGRAM,
        .ai_protocol = proto,
        .ai_flags = AI_NUMERICSERV | AI_IDN,
    }, *res;
    int       i_handle = -1;
    bool      b_unreach = false;
    if( i_hlim < 0 )
        i_hlim = var_InheritInteger( p_this, "ttl" );
    msg_Dbg( p_this, "net: connecting to [%s]:%d", psz_host, i_port );
    int val = vlc_getaddrinfo (psz_host, i_port, &hints, &res);
    if (val)
    {
        msg_Err (p_this, "cannot resolve [%s]:%d : %s", psz_host, i_port,
                 gai_strerror (val));
        return -1;
    }
    for (struct addrinfo *ptr = res; ptr != NULL; ptr = ptr->ai_next)
    {
        char *str;
        int fd = net_Socket (p_this, ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);
        if (fd == -1)
            continue;
        /* Allow broadcast sending */
        setsockopt (fd, SOL_SOCKET, SO_BROADCAST, &(int){ 1 }, sizeof (int));
        if( i_hlim >= 0 )
            net_SetMcastHopLimit( p_this, fd, ptr->ai_family, i_hlim );
        str = var_InheritString (p_this, "miface");
        if (str != NULL)
        {
            net_SetMcastOut (p_this, fd, ptr->ai_family, str);
            free (str);
        }
        net_SetDSCP (fd, var_InheritInteger (p_this, "dscp"));
        if( connect( fd, ptr->ai_addr, ptr->ai_addrlen ) == 0 )
        {
            /* success */
            i_handle = fd;
            break;
        }
#if defined( _WIN32 )
        if( WSAGetLastError () == WSAENETUNREACH )
#else
        if( errno == ENETUNREACH )
#endif
            b_unreach = true;
        else
            msg_Warn( p_this, "%s port %d : %s", psz_host, i_port,
                      vlc_strerror_c(errno) );
        net_Close( fd );
    }
    freeaddrinfo( res );
    if( i_handle == -1 )
    {
        if( b_unreach )
            msg_Err( p_this, "Host %s port %d is unreachable", psz_host,
                     i_port );
        return -1;
    }
    return i_handle;
}
