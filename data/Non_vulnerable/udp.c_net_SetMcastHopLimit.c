}
static int net_SetMcastHopLimit( vlc_object_t *p_this,
                                 int fd, int family, int hlim )
{
    int proto, cmd;
    /* There is some confusion in the world whether IP_MULTICAST_TTL
     * takes a byte or an int as an argument.
     * BSD seems to indicate byte so we are going with that and use
     * int as a fallback to be safe */
    switch( family )
    {
#ifdef IP_MULTICAST_TTL
        case AF_INET:
            proto = SOL_IP;
            cmd = IP_MULTICAST_TTL;
            break;
#endif
#ifdef IPV6_MULTICAST_HOPS
        case AF_INET6:
            proto = SOL_IPV6;
            cmd = IPV6_MULTICAST_HOPS;
            break;
#endif
        default:
            errno = EAFNOSUPPORT;
            msg_Warn( p_this, "%s", vlc_strerror_c(EAFNOSUPPORT) );
            return VLC_EGENERIC;
    }
    if( setsockopt( fd, proto, cmd, &hlim, sizeof( hlim ) ) < 0 )
    {
        /* BSD compatibility */
        unsigned char buf;
        msg_Dbg( p_this, "cannot set hop limit (%d): %s", hlim,
                 vlc_strerror_c(net_errno) );
        buf = (unsigned char)(( hlim > 255 ) ? 255 : hlim);
        if( setsockopt( fd, proto, cmd, &buf, sizeof( buf ) ) )
        {
            msg_Err( p_this, "cannot set hop limit (%d): %s", hlim,
                     vlc_strerror_c(net_errno) );
            return VLC_EGENERIC;
        }
    }
    return VLC_SUCCESS;
}
