 ****************************************************************************/
static block_t *Encode( encoder_t *p_enc, picture_t *p_pict )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    x264_picture_t pic;
    x264_nal_t *nal;
    block_t *p_block;
    int i_nal=0, i_out=0, i=0;
    /* init pic */
#if X264_BUILD >= 98
    x264_picture_init( &pic );
#else
    memset( &pic, 0, sizeof( x264_picture_t ) );
#endif
    if( likely(p_pict) ) {
       pic.i_pts = p_pict->date;
       pic.img.i_csp = p_sys->i_colorspace;
       pic.img.i_plane = p_pict->i_planes;
       for( i = 0; i < p_pict->i_planes; i++ )
       {
           pic.img.plane[i] = p_pict->p[i].p_pixels;
           pic.img.i_stride[i] = p_pict->p[i].i_pitch;
       }
       x264_encoder_encode( p_sys->h, &nal, &i_nal, &pic, &pic );
    } else {
       if( x264_encoder_delayed_frames( p_sys->h ) ) {
           x264_encoder_encode( p_sys->h, &nal, &i_nal, NULL, &pic );
       }
    }
    if( !i_nal ) return NULL;
    /* Get size of block we need */
    for( i = 0; i < i_nal; i++ )
        i_out += nal[i].i_payload;
    p_block = block_Alloc( i_out + p_sys->i_sei_size );
    if( !p_block ) return NULL;
    unsigned int i_offset = 0;
    if( unlikely( p_sys->i_sei_size ) )
    {
       /* insert x264 headers SEI nal into the first picture block at the start */
       memcpy( p_block->p_buffer, p_sys->p_sei, p_sys->i_sei_size );
       i_offset = p_sys->i_sei_size;
       p_sys->i_sei_size = 0;
       free( p_sys->p_sei );
       p_sys->p_sei = NULL;
    }
    /* copy encoded data directly to block */
    memcpy( p_block->p_buffer + i_offset, nal[0].p_payload, i_out );
    if( pic.b_keyframe )
        p_block->i_flags |= BLOCK_FLAG_TYPE_I;
    else if( pic.i_type == X264_TYPE_P || pic.i_type == X264_TYPE_I )
        p_block->i_flags |= BLOCK_FLAG_TYPE_P;
    else if( IS_X264_TYPE_B( pic.i_type ) )
        p_block->i_flags |= BLOCK_FLAG_TYPE_B;
    else
        p_block->i_flags |= BLOCK_FLAG_TYPE_PB;
    /* This isn't really valid for streams with B-frames */
    p_block->i_length = INT64_C(1000000) *
        p_enc->fmt_in.video.i_frame_rate_base /
            p_enc->fmt_in.video.i_frame_rate;
    /* scale pts-values back*/
    p_block->i_pts = pic.i_pts;
    p_block->i_dts = pic.i_dts;
    return p_block;
}
