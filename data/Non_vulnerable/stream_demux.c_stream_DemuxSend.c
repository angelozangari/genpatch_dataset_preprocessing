}
void stream_DemuxSend( stream_t *s, block_t *p_block )
{
    stream_sys_t *p_sys = s->p_sys;
    block_FifoPut( p_sys->p_fifo, p_block );
}
