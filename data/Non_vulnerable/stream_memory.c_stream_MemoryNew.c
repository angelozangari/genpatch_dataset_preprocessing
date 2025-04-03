 */
stream_t *stream_MemoryNew( vlc_object_t *p_this, uint8_t *p_buffer,
                            uint64_t i_size, bool i_preserve_memory )
{
    stream_t *s = stream_CommonNew( p_this );
    stream_sys_t *p_sys;
    if( !s )
        return NULL;
    s->psz_path = strdup( "" ); /* N/A */
    s->p_sys = p_sys = malloc( sizeof( stream_sys_t ) );
    if( !s->psz_path || !s->p_sys )
    {
        stream_CommonDelete( s );
        return NULL;
    }
    p_sys->i_pos = 0;
    p_sys->i_size = i_size;
    p_sys->p_buffer = p_buffer;
    p_sys->i_preserve_memory = i_preserve_memory;
    s->pf_read    = Read;
    s->pf_peek    = Peek;
    s->pf_control = Control;
    s->pf_destroy = Delete;
    s->p_input = NULL;
    return s;
}
