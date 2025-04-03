}
static block_t *DoWork( filter_t *p_filter, block_t *p_in_buf )
{
    block_t *block = block_Duplicate( p_in_buf );
    if( likely(block != NULL) )
        block_FifoPut( p_filter->p_sys->fifo, block );
    return p_in_buf;
}
