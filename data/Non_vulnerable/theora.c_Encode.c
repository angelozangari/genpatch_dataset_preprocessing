 ****************************************************************************/
static block_t *Encode( encoder_t *p_enc, picture_t *p_pict )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    ogg_packet oggpacket;
    block_t *p_block;
    th_ycbcr_buffer ycbcr;
    int i;
    if( !p_pict ) return NULL;
    /* Sanity check */
    if( p_pict->p[0].i_pitch < (int)p_sys->i_width ||
        p_pict->p[0].i_lines < (int)p_sys->i_height )
    {
        msg_Warn( p_enc, "frame is smaller than encoding size"
                  "(%ix%i->%ix%i) -> dropping frame",
                  p_pict->p[0].i_pitch, p_pict->p[0].i_lines,
                  p_sys->i_width, p_sys->i_height );
        return NULL;
    }
    /* Fill padding */
    if( p_pict->p[0].i_visible_pitch < (int)p_sys->i_width )
    {
        for( i = 0; i < p_sys->i_height; i++ )
        {
            memset( p_pict->p[0].p_pixels + i * p_pict->p[0].i_pitch +
                    p_pict->p[0].i_visible_pitch,
                    *( p_pict->p[0].p_pixels + i * p_pict->p[0].i_pitch +
                       p_pict->p[0].i_visible_pitch - 1 ),
                    p_sys->i_width - p_pict->p[0].i_visible_pitch );
        }
        for( i = 0; i < p_sys->i_height / 2; i++ )
        {
            memset( p_pict->p[1].p_pixels + i * p_pict->p[1].i_pitch +
                    p_pict->p[1].i_visible_pitch,
                    *( p_pict->p[1].p_pixels + i * p_pict->p[1].i_pitch +
                       p_pict->p[1].i_visible_pitch - 1 ),
                    p_sys->i_width / 2 - p_pict->p[1].i_visible_pitch );
            memset( p_pict->p[2].p_pixels + i * p_pict->p[2].i_pitch +
                    p_pict->p[2].i_visible_pitch,
                    *( p_pict->p[2].p_pixels + i * p_pict->p[2].i_pitch +
                       p_pict->p[2].i_visible_pitch - 1 ),
                    p_sys->i_width / 2 - p_pict->p[2].i_visible_pitch );
        }
    }
    if( p_pict->p[0].i_visible_lines < (int)p_sys->i_height )
    {
        for( i = p_pict->p[0].i_visible_lines; i < p_sys->i_height; i++ )
        {
            memset( p_pict->p[0].p_pixels + i * p_pict->p[0].i_pitch, 0,
                    p_sys->i_width );
        }
        for( i = p_pict->p[1].i_visible_lines; i < p_sys->i_height / 2; i++ )
        {
            memset( p_pict->p[1].p_pixels + i * p_pict->p[1].i_pitch, 0x80,
                    p_sys->i_width / 2 );
            memset( p_pict->p[2].p_pixels + i * p_pict->p[2].i_pitch, 0x80,
                    p_sys->i_width / 2 );
        }
    }
    /* Theora is a one-frame-in, one-frame-out system. Submit a frame
     * for compression and pull out the packet. */
    ycbcr[0].width = p_sys->i_width;
    ycbcr[0].height = p_sys->i_height;
    ycbcr[0].stride = p_pict->p[0].i_pitch;
    ycbcr[0].data = p_pict->p[0].p_pixels;
    ycbcr[1].width = p_sys->i_width / 2;
    ycbcr[1].height = p_sys->i_height / 2;
    ycbcr[1].stride = p_pict->p[1].i_pitch;
    ycbcr[1].data = p_pict->p[1].p_pixels;
    ycbcr[2].width = p_sys->i_width / 2;
    ycbcr[2].height = p_sys->i_height / 2;
    ycbcr[2].stride = p_pict->p[1].i_pitch;
    ycbcr[2].data = p_pict->p[2].p_pixels;
    if( th_encode_ycbcr_in( p_sys->tcx, ycbcr ) < 0 )
    {
        msg_Warn( p_enc, "failed encoding a frame" );
        return NULL;
    }
    th_encode_packetout( p_sys->tcx, 0, &oggpacket );
    /* Ogg packet to block */
    p_block = block_Alloc( oggpacket.bytes );
    memcpy( p_block->p_buffer, oggpacket.packet, oggpacket.bytes );
    p_block->i_dts = p_block->i_pts = p_pict->date;
    if( th_packet_iskeyframe( &oggpacket ) )
    {
        p_block->i_flags |= BLOCK_FLAG_TYPE_I;
    }
    return p_block;
}
