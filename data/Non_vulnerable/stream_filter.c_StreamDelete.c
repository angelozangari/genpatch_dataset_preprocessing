}
static void StreamDelete( stream_t *s )
{
    module_unneed( s, s->p_module );
    if( s->p_source )
        stream_Delete( s->p_source );
    stream_CommonDelete( s );
}
