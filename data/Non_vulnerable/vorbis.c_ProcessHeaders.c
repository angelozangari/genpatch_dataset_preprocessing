 *****************************************************************************/
static int ProcessHeaders( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    ogg_packet oggpacket;
    unsigned pi_size[XIPH_MAX_HEADER_COUNT];
    void *pp_data[XIPH_MAX_HEADER_COUNT];
    unsigned i_count;
    if( xiph_SplitHeaders( pi_size, pp_data, &i_count,
                           p_dec->fmt_in.i_extra, p_dec->fmt_in.p_extra) )
        return VLC_EGENERIC;
    if( i_count < 3 )
        return VLC_EGENERIC;
    oggpacket.granulepos = -1;
    oggpacket.e_o_s = 0;
    oggpacket.packetno = 0;
    /* Take care of the initial Vorbis header */
    oggpacket.b_o_s  = 1; /* yes this actually is a b_o_s packet :) */
    oggpacket.bytes  = pi_size[0];
    oggpacket.packet = pp_data[0];
    if( vorbis_synthesis_headerin( &p_sys->vi, &p_sys->vc, &oggpacket ) < 0 )
    {
        msg_Err( p_dec, "this bitstream does not contain Vorbis audio data");
        return VLC_EGENERIC;
    }
    /* Setup the format */
    p_dec->fmt_out.audio.i_rate     = p_sys->vi.rate;
    p_dec->fmt_out.audio.i_channels = p_sys->vi.channels;
    if( p_dec->fmt_out.audio.i_channels >= ARRAY_SIZE(pi_channels_maps) )
    {
        msg_Err( p_dec, "invalid number of channels (1-%zu): %i",
                 ARRAY_SIZE(pi_channels_maps),
                 p_dec->fmt_out.audio.i_channels );
        return VLC_EGENERIC;
    }
    p_dec->fmt_out.audio.i_physical_channels =
        p_dec->fmt_out.audio.i_original_channels =
            pi_channels_maps[p_sys->vi.channels];
    p_dec->fmt_out.i_bitrate = __MAX( 0, (int32_t) p_sys->vi.bitrate_nominal );
    date_Init( &p_sys->end_date, p_sys->vi.rate, 1 );
    msg_Dbg( p_dec, "channels:%d samplerate:%ld bitrate:%ud",
             p_sys->vi.channels, p_sys->vi.rate, p_dec->fmt_out.i_bitrate );
    /* The next packet in order is the comments header */
    oggpacket.b_o_s  = 0;
    oggpacket.bytes  = pi_size[1];
    oggpacket.packet = pp_data[1];
    if( vorbis_synthesis_headerin( &p_sys->vi, &p_sys->vc, &oggpacket ) < 0 )
    {
        msg_Err( p_dec, "2nd Vorbis header is corrupted" );
        return VLC_EGENERIC;
    }
    ParseVorbisComments( p_dec );
    /* The next packet in order is the codebooks header
     * We need to watch out that this packet is not missing as a
     * missing or corrupted header is fatal. */
    oggpacket.b_o_s  = 0;
    oggpacket.bytes  = pi_size[2];
    oggpacket.packet = pp_data[2];
    if( vorbis_synthesis_headerin( &p_sys->vi, &p_sys->vc, &oggpacket ) < 0 )
    {
        msg_Err( p_dec, "3rd Vorbis header is corrupted" );
        return VLC_EGENERIC;
    }
    if( !p_sys->b_packetizer )
    {
        /* Initialize the Vorbis packet->PCM decoder */
        vorbis_synthesis_init( &p_sys->vd, &p_sys->vi );
        vorbis_block_init( &p_sys->vd, &p_sys->vb );
    }
    else
    {
        p_dec->fmt_out.i_extra = p_dec->fmt_in.i_extra;
        p_dec->fmt_out.p_extra = xrealloc( p_dec->fmt_out.p_extra,
                                           p_dec->fmt_out.i_extra );
        memcpy( p_dec->fmt_out.p_extra,
                p_dec->fmt_in.p_extra, p_dec->fmt_out.i_extra );
    }
    ConfigureChannelOrder(p_sys->pi_chan_table, p_sys->vi.channels,
            p_dec->fmt_out.audio.i_physical_channels, true);
    return VLC_SUCCESS;
}
