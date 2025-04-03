 *****************************************************************************/
static void OptimizeForRead( int fd )
{
    /* cf. Open() in file access module */
    VLC_UNUSED(fd);
#ifdef HAVE_POSIX_FADVISE
    posix_fadvise( fd, 0, 4096, POSIX_FADV_WILLNEED );
    posix_fadvise( fd, 0, 0, POSIX_FADV_NOREUSE );
#endif
#ifdef F_RDAHEAD
    fcntl( fd, F_RDAHEAD, 1 );
#endif
#ifdef F_NOCACHE
    fcntl( fd, F_NOCACHE, 0 );
#endif
}
