}
static int net_SetMcastOut (vlc_object_t *p_this, int fd, int family,
                            const char *iface)
{
    int scope = if_nametoindex (iface);
    if (scope == 0)
    {
        msg_Err (p_this, "invalid multicast interface: %s", iface);
        return -1;
    }
    switch (family)
    {
#ifdef IPV6_MULTICAST_IF
        case AF_INET6:
            if (setsockopt (fd, SOL_IPV6, IPV6_MULTICAST_IF,
                            &scope, sizeof (scope)) == 0)
                return 0;
            break;
#endif
#ifdef __linux__
        case AF_INET:
        {
            struct ip_mreqn req = { .imr_ifindex = scope };
            if (setsockopt (fd, SOL_IP, IP_MULTICAST_IF,
                            &req, sizeof (req)) == 0)
                return 0;
            break;
        }
#endif
        default:
            errno = EAFNOSUPPORT;
    }
    msg_Err (p_this, "cannot force multicast interface %s: %s", iface,
             vlc_strerror_c(errno));
    return -1;
}
