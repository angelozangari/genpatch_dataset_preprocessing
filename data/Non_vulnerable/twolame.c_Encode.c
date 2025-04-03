}
static block_t *Encode( encoder_t *p_enc, block_t *p_aout_buf )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    block_t *p_chain = NULL;
    if( unlikely( !p_aout_buf ) ) {
        int i_used = 0;
        block_t *p_block;
        i_used = twolame_encode_flush( p_sys->p_twolame,
                                p_sys->p_out_buffer, MAX_CODED_FRAME_SIZE );
        if( i_used <= 0 )
            return NULL;
        p_block = block_Alloc( i_used );
        if( !p_block )
            return NULL;
        memcpy( p_block->p_buffer, p_sys->p_out_buffer, i_used );
        p_block->i_length = (mtime_t)1000000 *
                (mtime_t)MPEG_FRAME_SIZE / (mtime_t)p_enc->fmt_out.audio.i_rate;
        p_block->i_dts = p_block->i_pts = p_sys->i_pts;
        p_sys->i_pts += p_block->i_length;
        return p_block;
    }
    int16_t *p_buffer = (int16_t *)p_aout_buf->p_buffer;
    int i_nb_samples = p_aout_buf->i_nb_samples;
    p_sys->i_pts = p_aout_buf->i_pts -
                (mtime_t)1000000 * (mtime_t)p_sys->i_nb_samples /
                (mtime_t)p_enc->fmt_out.audio.i_rate;
    while ( p_sys->i_nb_samples + i_nb_samples >= MPEG_FRAME_SIZE )
    {
        int i_used;
        block_t *p_block;
        Bufferize( p_enc, p_buffer, MPEG_FRAME_SIZE - p_sys->i_nb_samples );
        i_nb_samples -= MPEG_FRAME_SIZE - p_sys->i_nb_samples;
        p_buffer += (MPEG_FRAME_SIZE - p_sys->i_nb_samples) * 2;
        i_used = twolame_encode_buffer_interleaved( p_sys->p_twolame,
                               p_sys->p_buffer, MPEG_FRAME_SIZE,
                               p_sys->p_out_buffer, MAX_CODED_FRAME_SIZE );
        /* On error, buffer samples and return what was already encoded */
        if( i_used < 0 )
        {
            msg_Err( p_enc, "encoder error: %d", i_used );
            break;
        }
        p_sys->i_nb_samples = 0;
        p_block = block_Alloc( i_used );
        if( !p_block )
        {
            if( p_chain )
                block_ChainRelease( p_chain );
            return NULL;
        }
        memcpy( p_block->p_buffer, p_sys->p_out_buffer, i_used );
        p_block->i_length = (mtime_t)1000000 *
                (mtime_t)MPEG_FRAME_SIZE / (mtime_t)p_enc->fmt_out.audio.i_rate;
        p_block->i_dts = p_block->i_pts = p_sys->i_pts;
        p_sys->i_pts += p_block->i_length;
        block_ChainAppend( &p_chain, p_block );
    }
    if ( i_nb_samples )
    {
        Bufferize( p_enc, p_buffer, i_nb_samples );
        p_sys->i_nb_samples += i_nb_samples;
    }
    return p_chain;
}
