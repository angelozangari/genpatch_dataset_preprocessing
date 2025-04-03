}
static int Peek( stream_t *s, const uint8_t **pp_peek, unsigned int i_read )
{
    stream_sys_t *p_sys = s->p_sys;
    int i_res = __MIN( i_read, p_sys->i_size - p_sys->i_pos );
    *pp_peek = p_sys->p_buffer + p_sys->i_pos;
    return i_res;
}
