/* ParseIDU: parse an Independent Decoding Unit */
static block_t *ParseIDU( decoder_t *p_dec, bool *pb_ts_used, block_t *p_frag )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_pic;
    const idu_type_t idu = p_frag->p_buffer[3];
    *pb_ts_used = false;
    if( !p_sys->b_sequence_header && idu != IDU_TYPE_SEQUENCE_HEADER )
    {
        msg_Warn( p_dec, "waiting for sequence header" );
        block_Release( p_frag );
        return NULL;
    }
    if( p_sys->b_sequence_header && !p_sys->b_entry_point && idu != IDU_TYPE_ENTRY_POINT )
    {
        msg_Warn( p_dec, "waiting for entry point" );
        block_Release( p_frag );
        return NULL;
    }
    /* TODO we do not gather ENTRY_POINT and SEQUENCE_DATA user data
     * But It should not be a problem for decoder */
    /* Do we have completed a frame */
    p_pic = NULL;
    if( p_sys->b_frame &&
        idu != IDU_TYPE_FRAME_USER_DATA &&
        idu != IDU_TYPE_FIELD && idu != IDU_TYPE_FIELD_USER_DATA &&
        idu != IDU_TYPE_SLICE && idu != IDU_TYPE_SLICE_USER_DATA &&
        idu != IDU_TYPE_END_OF_SEQUENCE )
    {
        /* Prepend SH and EP on I */
        if( p_sys->p_frame->i_flags & BLOCK_FLAG_TYPE_I )
        {
            block_t *p_list = block_Duplicate( p_sys->sh.p_sh );
            block_ChainAppend( &p_list, block_Duplicate( p_sys->ep.p_ep ) );
            block_ChainAppend( &p_list, p_sys->p_frame );
            p_list->i_flags = p_sys->p_frame->i_flags;
            p_sys->p_frame = p_list;
        }
        /* */
        p_pic = block_ChainGather( p_sys->p_frame );
        p_pic->i_dts = p_sys->i_frame_dts;
        p_pic->i_pts = p_sys->i_frame_pts;
        /* */
        if( p_pic->i_dts > VLC_TS_INVALID )
            p_sys->i_interpolated_dts = p_pic->i_dts;
        /* We can interpolate dts/pts only if we have a frame rate */
        if( p_dec->fmt_out.video.i_frame_rate != 0 && p_dec->fmt_out.video.i_frame_rate_base != 0 )
        {
            if( p_sys->i_interpolated_dts > VLC_TS_INVALID )
                p_sys->i_interpolated_dts += INT64_C(1000000) *
                                             p_dec->fmt_out.video.i_frame_rate_base /
                                             p_dec->fmt_out.video.i_frame_rate;
            //msg_Dbg( p_dec, "-------------- XXX0 dts=%"PRId64" pts=%"PRId64" interpolated=%"PRId64,
            //         p_pic->i_dts, p_pic->i_pts, p_sys->i_interpolated_dts );
            if( p_pic->i_dts <= VLC_TS_INVALID )
                p_pic->i_dts = p_sys->i_interpolated_dts;
            if( p_pic->i_pts <= VLC_TS_INVALID )
            {
                if( !p_sys->sh.b_has_bframe || (p_pic->i_flags & BLOCK_FLAG_TYPE_B ) )
                    p_pic->i_pts = p_pic->i_dts;
                /* TODO compute pts for other case */
            }
        }
        //msg_Dbg( p_dec, "-------------- dts=%"PRId64" pts=%"PRId64, p_pic->i_dts, p_pic->i_pts );
        /* Reset context */
        p_sys->b_frame = false;
        p_sys->i_frame_dts = VLC_TS_INVALID;
        p_sys->i_frame_pts = VLC_TS_INVALID;
        p_sys->p_frame = NULL;
        p_sys->pp_last = &p_sys->p_frame;
    }
    /*  */
    if( p_sys->i_frame_dts <= VLC_TS_INVALID && p_sys->i_frame_pts <= VLC_TS_INVALID )
    {
        p_sys->i_frame_dts = p_frag->i_dts;
        p_sys->i_frame_pts = p_frag->i_pts;
        *pb_ts_used = true;
    }
    /* We will add back SH and EP on I frames */
    block_t *p_release = NULL;
    if( idu != IDU_TYPE_SEQUENCE_HEADER && idu != IDU_TYPE_ENTRY_POINT )
        block_ChainLastAppend( &p_sys->pp_last, p_frag );
    else
        p_release = p_frag;
    /* Parse IDU */
    if( idu == IDU_TYPE_SEQUENCE_HEADER )
    {
        es_format_t *p_es = &p_dec->fmt_out;
        bs_t s;
        int i_profile;
        uint8_t ridu[32];
        int     i_ridu = sizeof(ridu);
        /* */
        if( p_sys->sh.p_sh )
            block_Release( p_sys->sh.p_sh );
        p_sys->sh.p_sh = block_Duplicate( p_frag );
        /* Extract the raw IDU */
        DecodeRIDU( ridu, &i_ridu, &p_frag->p_buffer[4], p_frag->i_buffer - 4 );
        /* Auto detect VC-1_SPMP_PESpacket_PayloadFormatHeader (SMPTE RP 227) for simple/main profile
         * TODO find a test case and valid it */
        if( i_ridu > 4 && (ridu[0]&0x80) == 0 ) /* for advanced profile, the first bit is 1 */
        {
            video_format_t *p_v = &p_dec->fmt_in.video;
            const size_t i_potential_width  = GetWBE( &ridu[0] );
            const size_t i_potential_height = GetWBE( &ridu[2] );
            if( i_potential_width >= 2  && i_potential_width <= 8192 &&
                i_potential_height >= 2 && i_potential_height <= 8192 )
            {
                if( ( p_v->i_width <= 0 && p_v->i_height <= 0  ) ||
                    ( p_v->i_width  == i_potential_width &&  p_v->i_height == i_potential_height ) )
                {
                    static const uint8_t startcode[4] = { 0x00, 0x00, 0x01, IDU_TYPE_SEQUENCE_HEADER };
                    p_es->video.i_width  = i_potential_width;
                    p_es->video.i_height = i_potential_height;
                    /* Remove it */
                    p_frag->p_buffer += 4;
                    p_frag->i_buffer -= 4;
                    memcpy( p_frag->p_buffer, startcode, sizeof(startcode) );
                }
            }
        }
        /* Parse it */
        bs_init( &s, ridu, i_ridu );
        i_profile = bs_read( &s, 2 );
        if( i_profile == 3 )
        {
            const int i_level = bs_read( &s, 3 );
            /* Advanced profile */
            p_sys->sh.b_advanced_profile = true;
            p_sys->sh.b_range_reduction = false;
            p_sys->sh.b_has_bframe = true;
            bs_skip( &s, 2+3+5+1 ); // chroma format + frame rate Q + bit rate Q + postprocflag
            p_es->video.i_width  = 2*bs_read( &s, 12 )+2;
            p_es->video.i_height = 2*bs_read( &s, 12 )+2;
            if( !p_sys->b_sequence_header )
                msg_Dbg( p_dec, "found sequence header for advanced profile level L%d resolution %dx%d",
                         i_level, p_es->video.i_width, p_es->video.i_height);
            bs_skip( &s, 1 );// pulldown
            p_sys->sh.b_interlaced = bs_read( &s, 1 );
            bs_skip( &s, 1 );// frame counter
            p_sys->sh.b_frame_interpolation = bs_read( &s, 1 );
            bs_skip( &s, 1 );       // Reserved
            bs_skip( &s, 1 );       // Psf
            if( bs_read( &s, 1 ) )  /* Display extension */
            {
                const int i_display_width  = bs_read( &s, 14 )+1;
                const int i_display_height = bs_read( &s, 14 )+1;
                p_es->video.i_sar_num = i_display_width  * p_es->video.i_height;
                p_es->video.i_sar_den = i_display_height * p_es->video.i_width;
                if( !p_sys->b_sequence_header )
                    msg_Dbg( p_dec, "display size %dx%d", i_display_width, i_display_height );
                if( bs_read( &s, 1 ) )  /* Pixel aspect ratio (PAR/SAR) */
                {
                    static const int p_ar[16][2] = {
                        { 0, 0}, { 1, 1}, {12,11}, {10,11}, {16,11}, {40,33},
                        {24,11}, {20,11}, {32,11}, {80,33}, {18,11}, {15,11},
                        {64,33}, {160,99},{ 0, 0}, { 0, 0}
                    };
                    int i_ar = bs_read( &s, 4 );
                    unsigned i_ar_w, i_ar_h;
                    if( i_ar == 15 )
                    {
                        i_ar_w = bs_read( &s, 8 );
                        i_ar_h = bs_read( &s, 8 );
                    }
                    else
                    {
                        i_ar_w = p_ar[i_ar][0];
                        i_ar_h = p_ar[i_ar][1];
                    }
                    vlc_ureduce( &i_ar_w, &i_ar_h, i_ar_w, i_ar_h, 0 );
                    if( !p_sys->b_sequence_header )
                        msg_Dbg( p_dec, "aspect ratio %d:%d", i_ar_w, i_ar_h );
                }
            }
            if( bs_read( &s, 1 ) )  /* Frame rate */
            {
                int i_fps_num = 0;
                int i_fps_den = 0;
                if( bs_read( &s, 1 ) )
                {
                    i_fps_num = bs_read( &s, 16 )+1;
                    i_fps_den = 32;
                }
                else
                {
                    const int i_nr = bs_read( &s, 8 );
                    const int i_dn = bs_read( &s, 4 );
                    switch( i_nr )
                    {
                    case 1: i_fps_num = 24000; break;
                    case 2: i_fps_num = 25000; break;
                    case 3: i_fps_num = 30000; break;
                    case 4: i_fps_num = 50000; break;
                    case 5: i_fps_num = 60000; break;
                    case 6: i_fps_num = 48000; break;
                    case 7: i_fps_num = 72000; break;
                    }
                    switch( i_dn )
                    {
                    case 1: i_fps_den = 1000; break;
                    case 2: i_fps_den = 1001; break;
                    }
                }
                if( i_fps_num != 0 && i_fps_den != 0 )
                    vlc_ureduce( &p_es->video.i_frame_rate, &p_es->video.i_frame_rate_base, i_fps_num, i_fps_den, 0 );
                if( !p_sys->b_sequence_header )
                    msg_Dbg( p_dec, "frame rate %d/%d", p_es->video.i_frame_rate, p_es->video.i_frame_rate_base );
            }
        }
        else
        {
            /* Simple and main profile */
            p_sys->sh.b_advanced_profile = false;
            p_sys->sh.b_interlaced = false;
            if( !p_sys->b_sequence_header )
                msg_Dbg( p_dec, "found sequence header for %s profile", i_profile == 0 ? "simple" : "main" );
            bs_skip( &s, 2+3+5+1+1+     // reserved + frame rate Q + bit rate Q + loop filter + reserved
                         1+1+1+1+2+     // multiresolution + reserved + fast uv mc + extended mv + dquant
                         1+1+1+1 );     // variable size transform + reserved + overlap + sync marker
            p_sys->sh.b_range_reduction = bs_read( &s, 1 );
            if( bs_read( &s, 3 ) > 0 )
                p_sys->sh.b_has_bframe = true;
            else
                p_sys->sh.b_has_bframe = false;
            bs_skip( &s, 2 );           // quantizer
            p_sys->sh.b_frame_interpolation = bs_read( &s, 1 );
        }
        p_sys->b_sequence_header = true;
        BuildExtraData( p_dec );
    }
    else if( idu == IDU_TYPE_ENTRY_POINT )
    {
        if( p_sys->ep.p_ep )
            block_Release( p_sys->ep.p_ep );
        p_sys->ep.p_ep = block_Duplicate( p_frag );
        if( !p_sys->b_entry_point )
            msg_Dbg( p_dec, "found entry point" );
        p_sys->b_entry_point = true;
        BuildExtraData( p_dec );
    }
    else if( idu == IDU_TYPE_FRAME )
    {
        bs_t s;
        uint8_t ridu[8];
        int     i_ridu = sizeof(ridu);
        /* Extract the raw IDU */
        DecodeRIDU( ridu, &i_ridu, &p_frag->p_buffer[4], p_frag->i_buffer - 4 );
        /* Parse it + interpolate pts/dts if possible */
        bs_init( &s, ridu, i_ridu );
        if( p_sys->sh.b_advanced_profile )
        {
            int i_fcm = 0;
            if( p_sys->sh.b_interlaced )
            {
                if( bs_read( &s, 1 ) )
                {
                    if( bs_read( &s, 1 ) )
                        i_fcm = 1;  /* interlaced field */
                    else
                        i_fcm = 2;  /* interlaced frame */
                }
            }
            if( i_fcm == 1 ) /*interlaced field */
            {
                /* XXX for mixed I/P we should check reference usage before marking them I (too much work) */
                switch( bs_read( &s, 3 ) )
                {
                case 0: /* II */
                case 1: /* IP */
                case 2: /* PI */
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_I;
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_I;
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_I;
                    break;
                case 3: /* PP */
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_P;
                    break;
                case 4: /* BB */
                case 5: /* BBi */
                case 6: /* BiB */
                case 7: /* BiBi */
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_B;
                    break;
                }
            }
            else
            {
                if( !bs_read( &s, 1 ) )
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_P;
                else if( !bs_read( &s, 1 ) )
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_B;
                else if( !bs_read( &s, 1 ) )
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_I;
                else if( !bs_read( &s, 1 ) )
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_B;   /* Bi */
                else
                    p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_P;   /* P Skip */
            }
        }
        else
        {
            if( p_sys->sh.b_frame_interpolation )
                bs_skip( &s, 1 );   // interpolate
            bs_skip( &s, 2 );       // frame count
            if( p_sys->sh.b_range_reduction )
                bs_skip( &s, 1 );   // range reduction
            if( bs_read( &s, 1 ) )
                p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_P;
            else if( !p_sys->sh.b_has_bframe || bs_read( &s, 1 ) )
                p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_I;
            else
                p_sys->p_frame->i_flags |= BLOCK_FLAG_TYPE_B;
        }
        p_sys->b_frame = true;
    }
    if( p_release )
        block_Release( p_release );
    return p_pic;
}
