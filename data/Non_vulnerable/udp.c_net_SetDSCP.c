}
static int net_SetDSCP( int fd, uint8_t dscp )
{
    struct sockaddr_storage addr;
    if( getsockname( fd, (struct sockaddr *)&addr, &(socklen_t){ sizeof (addr) }) )
        return -1;
    int level, cmd;
    switch( addr.ss_family )
    {
#ifdef IPV6_TCLASS
        case AF_INET6:
            level = SOL_IPV6;
            cmd = IPV6_TCLASS;
            break;
#endif
        case AF_INET:
            level = SOL_IP;
            cmd = IP_TOS;
            break;
        default:
#ifdef ENOPROTOOPT
            errno = ENOPROTOOPT;
#endif
            return -1;
    }
    return setsockopt( fd, level, cmd, &(int){ dscp }, sizeof (int));
}
