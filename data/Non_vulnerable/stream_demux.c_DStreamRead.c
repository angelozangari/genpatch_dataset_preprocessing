}
static int DStreamRead( stream_t *s, void *p_read, unsigned int i_read )
{
    stream_sys_t *p_sys = s->p_sys;
    uint8_t *p_out = p_read;
    int i_out = 0;
    //msg_Dbg( s, "DStreamRead: wanted %d bytes", i_read );
    while( atomic_load( &p_sys->active ) && !s->b_error && i_read )
    {
        block_t *p_block = p_sys->p_block;
        int i_copy;
        if( !p_block )
        {
            p_block = block_FifoGet( p_sys->p_fifo );
            if( !p_block ) s->b_error = 1;
            p_sys->p_block = p_block;
        }
        if( p_block && i_read )
        {
            i_copy = __MIN( i_read, p_block->i_buffer );
            if( p_out && i_copy ) memcpy( p_out, p_block->p_buffer, i_copy );
            i_read -= i_copy;
            p_out += i_copy;
            i_out += i_copy;
            p_block->i_buffer -= i_copy;
            p_block->p_buffer += i_copy;
            if( !p_block->i_buffer )
            {
                block_Release( p_block );
                p_sys->p_block = NULL;
            }
        }
    }
    p_sys->i_pos += i_out;
    return i_out;
}
