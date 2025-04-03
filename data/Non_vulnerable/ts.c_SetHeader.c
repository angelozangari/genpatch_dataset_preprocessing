}
static void SetHeader( sout_buffer_chain_t *c,
                        int depth )
{
    block_t *p_ts = BufferChainPeek( c );
    while( depth > 0 )
    {
        p_ts = p_ts->p_next;
        depth--;
    }
    p_ts->i_flags |= BLOCK_FLAG_HEADER;
}
