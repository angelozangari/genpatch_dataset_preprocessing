}
static int DStreamPeek( stream_t *s, const uint8_t **pp_peek, unsigned int i_peek )
{
    stream_sys_t *p_sys = s->p_sys;
    block_t **pp_block = &p_sys->p_block;
    int i_out = 0;
    *pp_peek = 0;
    //msg_Dbg( s, "DStreamPeek: wanted %d bytes", i_peek );
    while( atomic_load( &p_sys->active ) && !s->b_error && i_peek )
    {
        int i_copy;
        if( !*pp_block )
        {
            *pp_block = block_FifoGet( p_sys->p_fifo );
            if( !*pp_block ) s->b_error = 1;
        }
        if( *pp_block && i_peek )
        {
            i_copy = __MIN( i_peek, (*pp_block)->i_buffer );
            i_peek -= i_copy;
            i_out += i_copy;
            if( i_peek ) pp_block = &(*pp_block)->p_next;
        }
    }
    if( p_sys->p_block )
    {
        p_sys->p_block = block_ChainGather( p_sys->p_block );
        *pp_peek = p_sys->p_block->p_buffer;
    }
    return i_out;
}
