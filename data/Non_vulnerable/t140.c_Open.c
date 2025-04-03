static block_t *Encode ( encoder_t *, subpicture_t * );
static int Open( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    switch( p_enc->fmt_out.i_codec )
    {
        case VLC_CODEC_SUBT:
            if( ( p_enc->fmt_out.subs.psz_encoding != NULL )
             && strcasecmp( p_enc->fmt_out.subs.psz_encoding, "utf8" )
             && strcasecmp( p_enc->fmt_out.subs.psz_encoding, "UTF-8" ) )
            {
                msg_Err( p_this, "Only UTF-8 encoding supported" );
                return VLC_EGENERIC;
            }
        case VLC_CODEC_ITU_T140:
            break;
        default:
            if( !p_enc->b_force )
                return VLC_EGENERIC;
            p_enc->fmt_out.i_codec = VLC_CODEC_ITU_T140;
    }
    p_enc->p_sys = NULL;
    p_enc->pf_encode_sub = Encode;
    p_enc->fmt_out.i_cat = SPU_ES;
    return VLC_SUCCESS;
}
