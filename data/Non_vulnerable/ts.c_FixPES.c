#define STD_PES_PAYLOAD 170
static block_t *FixPES( sout_mux_t *p_mux, block_fifo_t *p_fifo )
{
    VLC_UNUSED(p_mux);
    block_t *p_data;
    size_t i_size;
    p_data = block_FifoShow( p_fifo );
    i_size = p_data->i_buffer;
    if( i_size == STD_PES_PAYLOAD )
    {
        return block_FifoGet( p_fifo );
    }
    else if( i_size > STD_PES_PAYLOAD )
    {
        block_t *p_new = block_Alloc( STD_PES_PAYLOAD );
        memcpy( p_new->p_buffer, p_data->p_buffer, STD_PES_PAYLOAD );
        p_new->i_pts = p_data->i_pts;
        p_new->i_dts = p_data->i_dts;
        p_new->i_length = p_data->i_length * STD_PES_PAYLOAD
                            / p_data->i_buffer;
        p_data->i_buffer -= STD_PES_PAYLOAD;
        p_data->p_buffer += STD_PES_PAYLOAD;
        p_data->i_pts += p_new->i_length;
        p_data->i_dts += p_new->i_length;
        p_data->i_length -= p_new->i_length;
        p_data->i_flags |= BLOCK_FLAG_NO_KEYFRAME;
        return p_new;
    }
    else
    {
        block_t *p_next;
        int i_copy;
        p_data = block_FifoGet( p_fifo );
        p_data = block_Realloc( p_data, 0, STD_PES_PAYLOAD );
        p_next = block_FifoShow( p_fifo );
        if ( p_data->i_flags & BLOCK_FLAG_NO_KEYFRAME )
        {
            p_data->i_flags &= ~BLOCK_FLAG_NO_KEYFRAME;
            p_data->i_pts = p_next->i_pts;
            p_data->i_dts = p_next->i_dts;
        }
        i_copy = __MIN( STD_PES_PAYLOAD - i_size, p_next->i_buffer );
        memcpy( &p_data->p_buffer[i_size], p_next->p_buffer, i_copy );
        p_next->i_pts += p_next->i_length * i_copy / p_next->i_buffer;
        p_next->i_dts += p_next->i_length * i_copy / p_next->i_buffer;
        p_next->i_length -= p_next->i_length * i_copy / p_next->i_buffer;
        p_next->i_buffer -= i_copy;
        p_next->p_buffer += i_copy;
        p_next->i_flags |= BLOCK_FLAG_NO_KEYFRAME;
        if( !p_next->i_buffer )
        {
            p_next = block_FifoGet( p_fifo );
            block_Release( p_next );
        }
        return p_data;
    }
}
