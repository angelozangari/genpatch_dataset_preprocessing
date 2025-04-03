 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t *) p_this;
    decoder_sys_t *p_sys = NULL;
    if( p_dec->fmt_in.i_codec != VLC_CODEC_TELETEXT )
        return VLC_EGENERIC;
    p_sys = p_dec->p_sys = calloc( 1, sizeof(decoder_sys_t) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->i_key[0] = p_sys->i_key[1] = p_sys->i_key[2] = '*' - '0';
    p_sys->b_update = false;
    p_sys->p_vbi_dec = vbi_decoder_new();
    vlc_mutex_init( &p_sys->lock );
    if( p_sys->p_vbi_dec == NULL )
    {
        msg_Err( p_dec, "VBI decoder could not be created." );
        Close( p_this );
        return VLC_ENOMEM;
    }
    /* Some broadcasters in countries with level 1 and level 1.5 still not send a G0 to do 
     * matches against table 32 of ETSI 300 706. We try to do some best effort guessing
     * This is not perfect, but might handle some cases where we know the vbi language 
     * is known. It would be better if people started sending G0 */
    for( int i = 0; ppsz_default_triplet[i] != NULL; i++ )
    {
        if( p_dec->fmt_in.psz_language && !strcasecmp( p_dec->fmt_in.psz_language, ppsz_default_triplet[i] ) )
        {
            vbi_teletext_set_default_region( p_sys->p_vbi_dec, pi_default_triplet[i]);
            msg_Dbg( p_dec, "overwriting default zvbi region: %d", pi_default_triplet[i] );
        }
    }
    vbi_event_handler_register( p_sys->p_vbi_dec, VBI_EVENT_TTX_PAGE | VBI_EVENT_NETWORK |
#ifdef ZVBI_DEBUG
                                VBI_EVENT_CAPTION | VBI_EVENT_TRIGGER |
                                VBI_EVENT_ASPECT | VBI_EVENT_PROG_INFO | VBI_EVENT_NETWORK_ID |
#endif
                                0 , EventHandler, p_dec );
    /* Create the var on vlc_global. */
    p_sys->i_wanted_page = var_CreateGetInteger( p_dec, "vbi-page" );
    var_AddCallback( p_dec, "vbi-page", RequestPage, p_sys );
    /* Check if the Teletext track has a known "initial page". */
    if( p_sys->i_wanted_page == 100 && p_dec->fmt_in.subs.teletext.i_magazine != -1 )
    {
        p_sys->i_wanted_page = 100 * p_dec->fmt_in.subs.teletext.i_magazine +
                               vbi_bcd2dec( p_dec->fmt_in.subs.teletext.i_page );
        var_SetInteger( p_dec, "vbi-page", p_sys->i_wanted_page );
    }
    p_sys->i_wanted_subpage = VBI_ANY_SUBNO;
    p_sys->b_opaque = var_CreateGetBool( p_dec, "vbi-opaque" );
    var_AddCallback( p_dec, "vbi-opaque", Opaque, p_sys );
    p_sys->i_align = var_CreateGetInteger( p_dec, "vbi-position" );
    var_AddCallback( p_dec, "vbi-position", Position, p_sys );
    p_sys->b_text = var_CreateGetBool( p_dec, "vbi-text" );
//    var_AddCallback( p_dec, "vbi-text", Text, p_sys );
    /* Listen for keys */
    var_AddCallback( p_dec->p_libvlc, "key-pressed", EventKey, p_dec );
    es_format_Init( &p_dec->fmt_out, SPU_ES, VLC_CODEC_SPU );
    if( p_sys->b_text )
        p_dec->fmt_out.video.i_chroma = VLC_CODEC_TEXT;
    else
        p_dec->fmt_out.video.i_chroma = VLC_CODEC_RGBA;
    p_dec->pf_decode_sub = Decode;
    return VLC_SUCCESS;
}
