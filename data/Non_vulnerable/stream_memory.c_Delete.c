}
static void Delete( stream_t *s )
{
    if( !s->p_sys->i_preserve_memory ) free( s->p_sys->p_buffer );
    free( s->p_sys );
    stream_CommonDelete( s );
}
