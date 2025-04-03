}
void net_ListenClose( int *pi_fd )
{
    if( pi_fd != NULL )
    {
        int *pi;
        for( pi = pi_fd; *pi != -1; pi++ )
            net_Close( *pi );
        free( pi_fd );
    }
}
