}
static block_t *WritePSISection( dvbpsi_psi_section_t* p_section )
{
    block_t   *p_psi, *p_first = NULL;
    while( p_section )
    {
        int i_size = (uint32_t)(p_section->p_payload_end - p_section->p_data) +
                  (p_section->b_syntax_indicator ? 4 : 0);
        p_psi = block_Alloc( i_size + 1 );
        if( !p_psi )
            goto error;
        p_psi->i_pts = 0;
        p_psi->i_dts = 0;
        p_psi->i_length = 0;
        p_psi->i_buffer = i_size + 1;
        p_psi->p_buffer[0] = 0; /* pointer */
        memcpy( p_psi->p_buffer + 1,
                p_section->p_data,
                i_size );
        block_ChainAppend( &p_first, p_psi );
        p_section = p_section->p_next;
    }
    return( p_first );
error:
    if( p_first )
        block_ChainRelease( p_first );
    return NULL;
}
