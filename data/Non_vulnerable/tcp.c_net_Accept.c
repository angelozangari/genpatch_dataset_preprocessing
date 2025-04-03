 */
int net_Accept (vlc_object_t *p_this, int *pi_fd)
{
    int evfd = vlc_object_waitpipe (p_this);
    assert (pi_fd != NULL);
    unsigned n = 0;
    while (pi_fd[n] != -1)
        n++;
    struct pollfd ufd[n + 1];
    /* Initialize file descriptor set */
    for (unsigned i = 0; i <= n; i++)
    {
        ufd[i].fd = (i < n) ? pi_fd[i] : evfd;
        ufd[i].events = POLLIN;
    }
    ufd[n].revents = 0;
    for (;;)
    {
        while (poll (ufd, n + (evfd != -1), -1) == -1)
        {
            if (net_errno != EINTR)
            {
                msg_Err (p_this, "poll error: %s", vlc_strerror_c(net_errno));
                return -1;
            }
        }
        for (unsigned i = 0; i < n; i++)
        {
            if (ufd[i].revents == 0)
                continue;
            int sfd = ufd[i].fd;
            int fd = net_AcceptSingle (p_this, sfd);
            if (fd == -1)
                continue;
            /*
             * Move listening socket to the end to let the others in the
             * set a chance next time.
             */
            memmove (pi_fd + i, pi_fd + i + 1, n - (i + 1));
            pi_fd[n - 1] = sfd;
            return fd;
        }
        if (ufd[n].revents)
        {
            errno = EINTR;
            break;
        }
    }
    return -1;
}
