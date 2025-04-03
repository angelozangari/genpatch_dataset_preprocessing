}
static inline void BufferChainClean( sout_buffer_chain_t *c )
{
    block_t *b;
    while( ( b = BufferChainGet( c ) ) )
    {
        block_Release( b );
    }
    BufferChainInit( c );
}
