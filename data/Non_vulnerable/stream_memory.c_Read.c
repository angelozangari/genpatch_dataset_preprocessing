}
static int Read( stream_t *s, void *p_read, unsigned int i_read )
{
    stream_sys_t *p_sys = s->p_sys;
    int i_res = __MIN( i_read, p_sys->i_size - p_sys->i_pos );
    if ( p_read )
        memcpy( p_read, p_sys->p_buffer + p_sys->i_pos, i_res );
    p_sys->i_pos += i_res;
    return i_res;
}
