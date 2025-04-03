static int
net_SourceSubscribe (vlc_object_t *obj, int fd,
                     const struct sockaddr *src, socklen_t srclen,
                     const struct sockaddr *grp, socklen_t grplen)
{
/* MCAST_JOIN_SOURCE_GROUP was introduced to OS X in v10.7, but it doesn't work,
 * so ignore it to use the same code path as on 10.5 or 10.6 */
#if defined (MCAST_JOIN_SOURCE_GROUP) && !defined (__APPLE__)
    /* Agnostic SSM multicast join */
    int level;
    struct group_source_req gsr;
    memset (&gsr, 0, sizeof (gsr));
    gsr.gsr_interface = var_GetIfIndex (obj);
    switch (grp->sa_family)
    {
#ifdef AF_INET6
        case AF_INET6:
        {
            const struct sockaddr_in6 *g6 = (const struct sockaddr_in6 *)grp;
            level = SOL_IPV6;
            assert (grplen >= sizeof (struct sockaddr_in6));
            if (g6->sin6_scope_id != 0)
                gsr.gsr_interface = g6->sin6_scope_id;
            break;
        }
#endif
        case AF_INET:
            level = SOL_IP;
            break;
        default:
            errno = EAFNOSUPPORT;
            return -1;
    }
    assert (grplen <= sizeof (gsr.gsr_group));
    memcpy (&gsr.gsr_source, src, srclen);
    assert (srclen <= sizeof (gsr.gsr_source));
    memcpy (&gsr.gsr_group,  grp, grplen);
    if (setsockopt (fd, level, MCAST_JOIN_SOURCE_GROUP,
                    &gsr, sizeof (gsr)) == 0)
        return 0;
#else
    if (src->sa_family != grp->sa_family)
    {
        errno = EAFNOSUPPORT;
        return -1;
    }
    switch (grp->sa_family)
    {
# ifdef IP_ADD_SOURCE_MEMBERSHIP
        /* IPv4-specific API */
        case AF_INET:
        {
            struct ip_mreq_source imr;
            memset (&imr, 0, sizeof (imr));
            assert (grplen >= sizeof (struct sockaddr_in));
            imr.imr_multiaddr = ((const struct sockaddr_in *)grp)->sin_addr;
            assert (srclen >= sizeof (struct sockaddr_in));
            imr.imr_sourceaddr = ((const struct sockaddr_in *)src)->sin_addr;
            if (setsockopt (fd, SOL_IP, IP_ADD_SOURCE_MEMBERSHIP,
                            &imr, sizeof (imr)) == 0)
                return 0;
            break;
        }
# endif
        default:
            errno = EAFNOSUPPORT;
    }
#endif
    msg_Err (obj, "cannot join source multicast group: %s",
             vlc_strerror_c(net_errno));
    msg_Warn (obj, "trying ASM instead of SSM...");
    return net_Subscribe (obj, fd, grp, grplen);
}
