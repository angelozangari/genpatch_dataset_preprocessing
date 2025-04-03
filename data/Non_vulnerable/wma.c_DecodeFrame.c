 *****************************************************************************/
static block_t *DecodeFrame( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    block_t       *p_aout_buffer = NULL;
    if( !pp_block || !*pp_block ) return NULL;
    p_block = *pp_block;
    if( p_block->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) )
    {
        date_Set( &p_sys->end_date, 0 );
        block_Release( p_block );
        *pp_block = NULL;
        return NULL;
    }
    if( p_block->i_buffer <= 0 )
    {
        /* we already decoded the samples, just feed a few to aout */
        if( p_sys->i_samples )
            p_aout_buffer = SplitBuffer( p_dec );
        if( !p_sys->i_samples )
        {   /* we need to decode new samples now */
            free( p_sys->p_output );
            p_sys->p_output = NULL;
            block_Release( p_block );
            *pp_block = NULL;
        }
        return p_aout_buffer;
    }
    /* Date management */
    if( p_block->i_pts > VLC_TS_INVALID &&
        p_block->i_pts != date_Get( &p_sys->end_date ) )
    {
        date_Set( &p_sys->end_date, p_block->i_pts );
        /* don't reuse the same pts */
        p_block->i_pts = VLC_TS_INVALID;
    }
    else if( !date_Get( &p_sys->end_date ) )
    {
        /* We've just started the stream, wait for the first PTS. */
        block_Release( p_block );
        return NULL;
    }
    if( wma_decode_superframe_init( &p_sys->wmadec, p_block->p_buffer,
            p_block->i_buffer ) == 0 )
    {
        msg_Err( p_dec, "failed initializing wmafixed decoder" );
        block_Release( p_block );
        *pp_block = NULL;
        return NULL;
    }
    if( p_sys->wmadec.nb_frames <= 0 )
    {
        msg_Err( p_dec, "can not decode, invalid ASF packet ?" );
        block_Release( p_block );
        *pp_block = NULL;
        return NULL;
    }
    /* worst case */
    size_t i_buffer = BLOCK_MAX_SIZE * MAX_CHANNELS * p_sys->wmadec.nb_frames;
    free( p_sys->p_output );
    p_sys->p_output = malloc(i_buffer * sizeof(int32_t) );
    p_sys->p_samples = (int8_t*)p_sys->p_output;
    if( !p_sys->p_output )
    {
        /* OOM, will try a bit later if VLC hasn't been killed */
        block_Release( p_block );
        return NULL;
    }
    p_sys->i_samples = 0;
    for( int i = 0 ; i < p_sys->wmadec.nb_frames; i++ )
    {
        int i_samples = 0;
        i_samples = wma_decode_superframe_frame( &p_sys->wmadec,
                 p_sys->p_output + p_sys->i_samples * p_sys->wmadec.nb_channels,
                 p_block->p_buffer, p_block->i_buffer );
        if( i_samples < 0 )
        {
            msg_Warn( p_dec,
                "wma_decode_superframe_frame() failed for frame %d", i );
            free( p_sys->p_output );
            p_sys->p_output = NULL;
            return NULL;
        }
        p_sys->i_samples += i_samples; /* advance in the samples buffer */
    }
    p_block->i_buffer = 0; /* this block has been decoded */
    for( size_t s = 0 ; s < i_buffer; s++ )
        p_sys->p_output[s] <<= 2; /* Q30 -> Q32 translation */
    p_aout_buffer = SplitBuffer( p_dec );
    assert( p_aout_buffer );
    return p_aout_buffer;
}
