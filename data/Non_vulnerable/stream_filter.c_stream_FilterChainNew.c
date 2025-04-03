}
stream_t *stream_FilterChainNew( stream_t *p_source,
                                 const char *psz_chain,
                                 bool b_record )
{
    /* Add auto stream filter */
    for( ;; )
    {
        stream_t *p_filter = stream_FilterNew( p_source, NULL );
        if( !p_filter )
            break;
        msg_Dbg( p_filter, "Inserted a stream filter" );
        p_source = p_filter;
    }
    /* Add user stream filter */
    char *psz_tmp = psz_chain ? strdup( psz_chain ) : NULL;
    char *psz = psz_tmp;
    while( psz && *psz )
    {
        stream_t *p_filter;
        char *psz_end = strchr( psz, ':' );
        if( psz_end )
            *psz_end++ = '\0';
        p_filter = stream_FilterNew( p_source, psz );
        if( p_filter )
            p_source = p_filter;
        else
            msg_Warn( p_source, "failed to insert stream filter %s", psz );
        psz = psz_end;
    }
    free( psz_tmp );
    /* Add record filter if useful */
    if( b_record )
    {
        stream_t *p_filter = stream_FilterNew( p_source, "record" );
        if( p_filter )
            p_source = p_filter;
    }
    return p_source;
}
