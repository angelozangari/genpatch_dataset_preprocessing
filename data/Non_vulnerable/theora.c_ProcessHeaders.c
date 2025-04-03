 *****************************************************************************/
static int ProcessHeaders( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    ogg_packet oggpacket;
    th_setup_info *ts = NULL; /* theora setup information */
    int i_max_pp, i_pp;
    unsigned pi_size[XIPH_MAX_HEADER_COUNT];
    void     *pp_data[XIPH_MAX_HEADER_COUNT];
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
    if( th_decode_headerin( &p_sys->ti, &p_sys->tc, &ts, &oggpacket ) < 0 )
    {
        msg_Err( p_dec, "this bitstream does not contain Theora video data" );
        goto error;
    }
    /* Set output properties */
    if( !p_sys->b_packetizer )
    switch( p_sys->ti.pixel_fmt )
    {
      case TH_PF_420:
        p_dec->fmt_out.i_codec = VLC_CODEC_I420;
        break;
      case TH_PF_422:
        p_dec->fmt_out.i_codec = VLC_CODEC_I422;
        break;
      case TH_PF_444:
        p_dec->fmt_out.i_codec = VLC_CODEC_I444;
        break;
      case TH_PF_RSVD:
      default:
        msg_Err( p_dec, "unknown chroma in theora sample" );
        break;
    }
    p_dec->fmt_out.video.i_width = p_sys->ti.frame_width;
    p_dec->fmt_out.video.i_height = p_sys->ti.frame_height;
    if( p_sys->ti.pic_width && p_sys->ti.pic_height )
    {
        p_dec->fmt_out.video.i_visible_width = p_sys->ti.pic_width;
        p_dec->fmt_out.video.i_visible_height = p_sys->ti.pic_height;
        if( p_sys->ti.pic_x || p_sys->ti.pic_y )
        {
            p_dec->fmt_out.video.i_x_offset = p_sys->ti.pic_x;
            p_dec->fmt_out.video.i_y_offset = p_sys->ti.pic_y;
        }
    }
    if( p_sys->ti.aspect_denominator && p_sys->ti.aspect_numerator )
    {
        p_dec->fmt_out.video.i_sar_num = p_sys->ti.aspect_numerator;
        p_dec->fmt_out.video.i_sar_den = p_sys->ti.aspect_denominator;
    }
    else
    {
        p_dec->fmt_out.video.i_sar_num = 1;
        p_dec->fmt_out.video.i_sar_den = 1;
    }
    if( p_sys->ti.fps_numerator > 0 && p_sys->ti.fps_denominator > 0 )
    {
        p_dec->fmt_out.video.i_frame_rate = p_sys->ti.fps_numerator;
        p_dec->fmt_out.video.i_frame_rate_base = p_sys->ti.fps_denominator;
    }
    msg_Dbg( p_dec, "%dx%d %.02f fps video, frame content "
             "is %dx%d with offset (%d,%d)",
             p_sys->ti.frame_width, p_sys->ti.frame_height,
             (double)p_sys->ti.fps_numerator/p_sys->ti.fps_denominator,
             p_sys->ti.pic_width, p_sys->ti.pic_height,
             p_sys->ti.pic_x, p_sys->ti.pic_y );
    /* Some assertions based on the documentation.  These are mandatory restrictions. */
    assert( p_sys->ti.frame_height % 16 == 0 && p_sys->ti.frame_height < 1048576 );
    assert( p_sys->ti.frame_width % 16 == 0 && p_sys->ti.frame_width < 1048576 );
    assert( p_sys->ti.keyframe_granule_shift >= 0 && p_sys->ti.keyframe_granule_shift <= 31 );
    assert( p_sys->ti.pic_x <= __MIN( p_sys->ti.frame_width - p_sys->ti.pic_width, 255 ) );
    assert( p_sys->ti.pic_y <= p_sys->ti.frame_height - p_sys->ti.pic_height);
    assert( p_sys->ti.frame_height - p_sys->ti.pic_height - p_sys->ti.pic_y <= 255 );
    /* Sanity check that seems necessary for some corrupted files */
    if( p_sys->ti.frame_width < p_sys->ti.pic_width ||
        p_sys->ti.frame_height < p_sys->ti.pic_height )
    {
        msg_Warn( p_dec, "trying to correct invalid theora header "
                  "(frame size (%dx%d) is smaller than frame content (%d,%d))",
                  p_sys->ti.frame_width, p_sys->ti.frame_height,
                  p_sys->ti.pic_width, p_sys->ti.pic_height );
        if( p_sys->ti.frame_width < p_sys->ti.pic_width )
          p_sys->ti.frame_width = p_sys->ti.pic_width;
        if( p_sys->ti.frame_height < p_sys->ti.pic_height )
            p_sys->ti.frame_height = p_sys->ti.pic_height;
    }
    /* The next packet in order is the comments header */
    oggpacket.b_o_s  = 0;
    oggpacket.bytes  = pi_size[1];
    oggpacket.packet = pp_data[1];
    if( th_decode_headerin( &p_sys->ti, &p_sys->tc, &ts, &oggpacket ) < 0 )
    {
        msg_Err( p_dec, "2nd Theora header is corrupted" );
        goto error;
    }
    ParseTheoraComments( p_dec );
    /* The next packet in order is the codebooks header
     * We need to watch out that this packet is not missing as a
     * missing or corrupted header is fatal. */
    oggpacket.b_o_s  = 0;
    oggpacket.bytes  = pi_size[2];
    oggpacket.packet = pp_data[2];
    if( th_decode_headerin( &p_sys->ti, &p_sys->tc, &ts, &oggpacket ) < 0 )
    {
        msg_Err( p_dec, "3rd Theora header is corrupted" );
        goto error;
    }
    if( !p_sys->b_packetizer )
    {
        /* We have all the headers, initialize decoder */
        if ( ( p_sys->tcx = th_decode_alloc( &p_sys->ti, ts ) ) == NULL )
        {
            msg_Err( p_dec, "Could not allocate Theora decoder" );
            goto error;
        }
        i_pp = var_InheritInteger( p_dec, DEC_CFG_PREFIX "postproc" );
        if ( i_pp >= 0 && !th_decode_ctl( p_sys->tcx,
                    TH_DECCTL_GET_PPLEVEL_MAX, &i_max_pp, sizeof(int) ) )
        {
            i_pp = __MIN( i_pp, i_max_pp );
            if ( th_decode_ctl( p_sys->tcx, TH_DECCTL_SET_PPLEVEL,
                                &i_pp, sizeof(int) ) )
                msg_Err( p_dec, "Failed to set post processing level to %d",
                         i_pp );
            else
                msg_Dbg( p_dec, "Set post processing level to %d / %d",
                         i_pp, i_max_pp );
        }
    }
    else
    {
        p_dec->fmt_out.i_extra = p_dec->fmt_in.i_extra;
        p_dec->fmt_out.p_extra = xrealloc( p_dec->fmt_out.p_extra,
                                                  p_dec->fmt_out.i_extra );
        memcpy( p_dec->fmt_out.p_extra,
                p_dec->fmt_in.p_extra, p_dec->fmt_out.i_extra );
    }
    /* Clean up the decoder setup info... we're done with it */
    th_setup_free( ts );
    return VLC_SUCCESS;
error:
    /* Clean up the decoder setup info... we're done with it */
    th_setup_free( ts );
    return VLC_EGENERIC;
}
