}
static inline void BufferChainAppend( sout_buffer_chain_t *c, block_t *b )
{
    *c->pp_last = b;
    c->i_depth++;
    while( b->p_next )
    {
        b = b->p_next;
        c->i_depth++;
    }
    c->pp_last = &b->p_next;
}
