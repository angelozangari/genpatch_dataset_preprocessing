}
static inline block_t *BufferChainGet( sout_buffer_chain_t *c )
{
    block_t *b = c->p_first;
    if( b )
    {
        c->i_depth--;
        c->p_first = b->p_next;
        if( c->p_first == NULL )
        {
            c->pp_last = &c->p_first;
        }
        b->p_next = NULL;
    }
    return b;
}
