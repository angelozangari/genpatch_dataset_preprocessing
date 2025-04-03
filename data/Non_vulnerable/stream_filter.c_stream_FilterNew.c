static void StreamDelete( stream_t * );
stream_t *stream_FilterNew( stream_t *p_source,
                            const char *psz_stream_filter )
{
    stream_t *s;
    assert( p_source != NULL );
    s = stream_CommonNew( VLC_OBJECT( p_source ) );
    if( s == NULL )
        return NULL;
    s->p_input = p_source->p_input;
    /* */
    s->psz_access = strdup( p_source->psz_access );
    s->psz_path = strdup( p_source->psz_path );
    if( !s->psz_path )
    {
        stream_CommonDelete( s );
        return NULL;
    }
    s->p_source = p_source;
    /* */
    s->p_module = module_need( s, "stream_filter", psz_stream_filter, true );
    if( !s->p_module )
    {
        stream_CommonDelete( s );
        return NULL;
    }
    s->pf_destroy = StreamDelete;
    return s;
}
