}
int net_AcceptSingle (vlc_object_t *obj, int lfd)
{
    int fd = vlc_accept (lfd, NULL, NULL, true);
    if (fd == -1)
    {
        if (net_errno != EAGAIN && net_errno != EWOULDBLOCK)
            msg_Err (obj, "accept failed (from socket %d): %s", lfd,
                     vlc_strerror_c(net_errno));
        return -1;
    }
    msg_Dbg (obj, "accepted socket %d (from socket %d)", fd, lfd);
    setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    return fd;
}
