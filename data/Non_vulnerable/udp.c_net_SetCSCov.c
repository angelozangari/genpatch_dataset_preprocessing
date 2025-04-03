 */
int net_SetCSCov (int fd, int sendcov, int recvcov)
{
    int type;
    if (getsockopt (fd, SOL_SOCKET, SO_TYPE,
                    &type, &(socklen_t){ sizeof (type) }))
        return VLC_EGENERIC;
    switch (type)
    {
#ifdef UDPLITE_RECV_CSCOV
        case SOCK_DGRAM: /* UDP-Lite */
            if (sendcov == -1)
                sendcov = 0;
            else
                sendcov += 8; /* partial */
            if (setsockopt (fd, SOL_UDPLITE, UDPLITE_SEND_CSCOV, &sendcov,
                            sizeof (sendcov)))
                return VLC_EGENERIC;
            if (recvcov == -1)
                recvcov = 0;
            else
                recvcov += 8;
            if (setsockopt (fd, SOL_UDPLITE, UDPLITE_RECV_CSCOV,
                            &recvcov, sizeof (recvcov)))
                return VLC_EGENERIC;
            return VLC_SUCCESS;
#endif
#ifdef DCCP_SOCKOPT_SEND_CSCOV
        case SOCK_DCCP: /* DCCP and its ill-named socket type */
            if ((sendcov == -1) || (sendcov > 56))
                sendcov = 0;
            else
                sendcov = (sendcov + 3) / 4;
            if (setsockopt (fd, SOL_DCCP, DCCP_SOCKOPT_SEND_CSCOV,
                            &sendcov, sizeof (sendcov)))
                return VLC_EGENERIC;
            if ((recvcov == -1) || (recvcov > 56))
                recvcov = 0;
            else
                recvcov = (recvcov + 3) / 4;
            if (setsockopt (fd, SOL_DCCP, DCCP_SOCKOPT_RECV_CSCOV,
                            &recvcov, sizeof (recvcov)))
                return VLC_EGENERIC;
            return VLC_SUCCESS;
#endif
    }
#if !defined( UDPLITE_RECV_CSCOV ) && !defined( DCCP_SOCKOPT_SEND_CSCOV )
    VLC_UNUSED(sendcov);
    VLC_UNUSED(recvcov);
#endif
    return VLC_EGENERIC;
}
