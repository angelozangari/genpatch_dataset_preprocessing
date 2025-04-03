/* */
static int net_SetupDgramSocket (vlc_object_t *p_obj, int fd,
                                 const struct addrinfo *ptr)
{
#ifdef SO_REUSEPORT
    setsockopt (fd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof (int));
#endif
#if defined (_WIN32)
    /* Check windows version so we know if we need to increase receive buffers
     * for Windows 7 and earlier
     * SetSocketMediaStreamingMode is present in win 8 and later, so we set
     * receive buffer if that isn't present
     */
    HINSTANCE h_Network = LoadLibraryW(L"Windows.Networking.dll");
    if( (h_Network == NULL) ||
        (GetProcAddress( h_Network, "SetSocketMediaStreamingMode" ) == NULL ) )
    {
        setsockopt (fd, SOL_SOCKET, SO_RCVBUF,
                         (void *)&(int){ 0x80000 }, sizeof (int));
    }
    if( h_Network )
        FreeLibrary( h_Network );
    if (net_SockAddrIsMulticast (ptr->ai_addr, ptr->ai_addrlen)
     && (sizeof (struct sockaddr_storage) >= ptr->ai_addrlen))
    {
        // This works for IPv4 too - don't worry!
        struct sockaddr_in6 dumb =
        {
            .sin6_family = ptr->ai_addr->sa_family,
            .sin6_port =  ((struct sockaddr_in *)(ptr->ai_addr))->sin_port
        };
        bind (fd, (struct sockaddr *)&dumb, ptr->ai_addrlen);
    }
    else
#endif
    if (bind (fd, ptr->ai_addr, ptr->ai_addrlen))
    {
        msg_Err( p_obj, "socket bind error: %s", vlc_strerror_c(net_errno) );
        net_Close (fd);
        return -1;
    }
    return fd;
}
