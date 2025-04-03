 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    if( p_dec->fmt_in.i_codec !=  VLC_CODEC_VC1 )
        return VLC_EGENERIC;
    p_dec->pf_packetize = Packetize;
    /* Create the output format */
    es_format_Copy( &p_dec->fmt_out, &p_dec->fmt_in );
    p_dec->p_sys = p_sys = malloc( sizeof( decoder_sys_t ) );
    if( unlikely( !p_sys ) )
        return VLC_ENOMEM;
    packetizer_Init( &p_sys->packetizer,
                     p_vc1_startcode, sizeof(p_vc1_startcode),
                     NULL, 0, 4,
                     PacketizeReset, PacketizeParse, PacketizeValidate, p_dec );
    p_sys->b_sequence_header = false;
    p_sys->sh.p_sh = NULL;
    p_sys->b_entry_point = false;
    p_sys->ep.p_ep = NULL;
    p_sys->i_frame_dts = VLC_TS_INVALID;
    p_sys->i_frame_pts = VLC_TS_INVALID;
    p_sys->b_frame = false;
    p_sys->p_frame = NULL;
    p_sys->pp_last = &p_sys->p_frame;
    p_sys->i_interpolated_dts = VLC_TS_INVALID;
    p_sys->b_check_startcode = p_dec->fmt_in.b_packetized;
    if( p_dec->fmt_out.i_extra > 0 )
    {
        uint8_t *p_extra = p_dec->fmt_out.p_extra;
        /* With (some) ASF the first byte has to be stripped */
        if( p_extra[0] != 0x00 )
        {
            memmove( &p_extra[0], &p_extra[1], p_dec->fmt_out.i_extra - 1 );
            p_dec->fmt_out.i_extra--;
        }
        /* */
        if( p_dec->fmt_out.i_extra > 0 )
            packetizer_Header( &p_sys->packetizer,
                               p_dec->fmt_out.p_extra, p_dec->fmt_out.i_extra );
    }
    return VLC_SUCCESS;
}
