}
static void* EncoderThread( void *obj )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t*)obj;
    sout_stream_id_sys_t *id = p_sys->id_video;
    picture_t *p_pic = NULL;
    int canc = vlc_savecancel ();
    block_t *p_block = NULL;
    for( ;; )
    {
        vlc_mutex_lock( &p_sys->lock_out );
        while( !p_sys->b_abort &&
               (p_pic = picture_fifo_Pop( p_sys->pp_pics )) == NULL )
            vlc_cond_wait( &p_sys->cond, &p_sys->lock_out );
        if( p_sys->b_abort && !p_pic )
        {
            vlc_mutex_unlock( &p_sys->lock_out );
            break;
        }
        vlc_mutex_unlock( &p_sys->lock_out );
        if( p_pic )
        {
            p_block = id->p_encoder->pf_encode_video( id->p_encoder, p_pic );
            vlc_mutex_lock( &p_sys->lock_out );
            block_ChainAppend( &p_sys->p_buffers, p_block );
            vlc_mutex_unlock( &p_sys->lock_out );
            picture_Release( p_pic );
        }
        vlc_mutex_lock( &p_sys->lock_out );
        if( p_sys->b_abort )
        {
            vlc_mutex_unlock( &p_sys->lock_out );
            break;
        }
        vlc_mutex_unlock( &p_sys->lock_out );
    }
    /*Encode what we have in the buffer on closing*/
    vlc_mutex_lock( &p_sys->lock_out );
    while( (p_pic = picture_fifo_Pop( p_sys->pp_pics )) != NULL )
    {
        p_block = id->p_encoder->pf_encode_video( id->p_encoder, p_pic );
        block_ChainAppend( &p_sys->p_buffers, p_block );
        picture_Release( p_pic );
    }
    /*Now flush encoder*/
    do {
       p_block = id->p_encoder->pf_encode_video(id->p_encoder, NULL );
       block_ChainAppend( &p_sys->p_buffers, p_block );
    } while( p_block );
    vlc_mutex_unlock( &p_sys->lock_out );
    vlc_restorecancel (canc);
    return NULL;
}
