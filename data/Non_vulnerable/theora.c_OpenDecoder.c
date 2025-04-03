 *****************************************************************************/
static int OpenDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_THEORA )
    {
        return VLC_EGENERIC;
    }
    /* Allocate the memory needed to store the decoder's structure */
    if( ( p_dec->p_sys = p_sys = malloc(sizeof(*p_sys)) ) == NULL )
        return VLC_ENOMEM;
    p_dec->p_sys->b_packetizer = false;
    p_sys->b_has_headers = false;
    p_sys->i_pts = VLC_TS_INVALID;
    p_sys->b_decoded_first_keyframe = false;
    p_sys->tcx = NULL;
    /* Set output properties */
    p_dec->fmt_out.i_cat = VIDEO_ES;
    p_dec->fmt_out.i_codec = VLC_CODEC_I420;
    /* Set callbacks */
    p_dec->pf_decode_video = (picture_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    p_dec->pf_packetize    = (block_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    /* Init supporting Theora structures needed in header parsing */
    th_comment_init( &p_sys->tc );
    th_info_init( &p_sys->ti );
    return VLC_SUCCESS;
}
