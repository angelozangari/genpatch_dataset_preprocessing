}
static inline block_t *BufferChainPeek( sout_buffer_chain_t *c )
{
    block_t *b = c->p_first;
    return b;
}
