 *****************************************************************************/
static picture_t *DecodePacket( decoder_t *p_dec, ogg_packet *p_oggpacket )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    picture_t *p_pic;
    th_ycbcr_buffer ycbcr;
    /* TODO: Implement _granpos (3rd parameter here) and add the
     * call to TH_DECCTL_SET_GRANDPOS after seek */
    /* TODO: If the return is TH_DUPFRAME, we don't need to display a new
     * frame, but we do need to keep displaying the previous one. */
    if (th_decode_packetin( p_sys->tcx, p_oggpacket, NULL ) < 0)
        return NULL; /* bad packet */
    /* Check for keyframe */
    if( !(p_oggpacket->packet[0] & 0x80) /* data packet */ &&
        !(p_oggpacket->packet[0] & 0x40) /* intra frame */ )
        p_sys->b_decoded_first_keyframe = true;
    /* If we haven't seen a single keyframe yet, don't let Theora decode
     * anything, otherwise we'll get display artifacts.  (This is impossible
     * in the general case, but can happen if e.g. we play a network stream
     * using a timed URL, such that the server doesn't start the video with a
     * keyframe). */
    if( !p_sys->b_decoded_first_keyframe )
        return NULL; /* Wait until we've decoded the first keyframe */
    if( th_decode_ycbcr_out( p_sys->tcx, ycbcr ) ) /* returns 0 on success */
        return NULL;
    /* Get a new picture */
    p_pic = decoder_NewPicture( p_dec );
    if( !p_pic ) return NULL;
    theora_CopyPicture( p_pic, ycbcr );
    p_pic->date = p_sys->i_pts;
    return p_pic;
}
