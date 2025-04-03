}
static void GetPMTmpeg4(sout_mux_t *p_mux)
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    uint8_t iod[4096];
    bits_buffer_t bits, bits_fix_IOD;
    /* Make valgrind happy : it works at byte level not bit one so
     * bit_write confuse it (but DON'T CHANGE the way that bit_write is
     * working (needed when fixing some bits) */
    memset( iod, 0, 4096 );
    bits_initwrite( &bits, 4096, iod );
    /* IOD_label_scope */
    bits_write( &bits, 8,   0x11 );
    /* IOD_label */
    bits_write( &bits, 8,   0x01 );
    /* InitialObjectDescriptor */
    bits_align( &bits );
    bits_write( &bits, 8,   0x02 );     /* tag */
    bits_fix_IOD = bits;    /* save states to fix length later */
    bits_write( &bits, 24,
        GetDescriptorLength24b( 0 ) );  /* variable length (fixed later) */
    bits_write( &bits, 10,  0x01 );     /* ObjectDescriptorID */
    bits_write( &bits, 1,   0x00 );     /* URL Flag */
    bits_write( &bits, 1,   0x00 );     /* includeInlineProfileLevelFlag */
    bits_write( &bits, 4,   0x0f );     /* reserved */
    bits_write( &bits, 8,   0xff );     /* ODProfile (no ODcapability ) */
    bits_write( &bits, 8,   0xff );     /* sceneProfile */
    bits_write( &bits, 8,   0xfe );     /* audioProfile (unspecified) */
    bits_write( &bits, 8,   0xfe );     /* visualProfile( // ) */
    bits_write( &bits, 8,   0xff );     /* graphicProfile (no ) */
    for (int i_stream = 0; i_stream < p_mux->i_nb_inputs; i_stream++ )
    {
        ts_stream_t *p_stream = (ts_stream_t*)p_mux->pp_inputs[i_stream]->p_sys;
        if( p_stream->i_stream_id != 0xfa && p_stream->i_stream_id != 0xfb &&
            p_stream->i_stream_id != 0xfe )
            continue;
        bits_buffer_t bits_fix_ESDescr, bits_fix_Decoder;
        /* ES descriptor */
        bits_align( &bits );
        bits_write( &bits, 8,   0x03 );     /* ES_DescrTag */
        bits_fix_ESDescr = bits;
        bits_write( &bits, 24,
                    GetDescriptorLength24b( 0 ) ); /* variable size */
        bits_write( &bits, 16,  p_stream->i_es_id );
        bits_write( &bits, 1,   0x00 );     /* streamDependency */
        bits_write( &bits, 1,   0x00 );     /* URL Flag */
        bits_write( &bits, 1,   0x00 );     /* OCRStreamFlag */
        bits_write( &bits, 5,   0x1f );     /* streamPriority */
        /* DecoderConfigDesciptor */
        bits_align( &bits );
        bits_write( &bits, 8,   0x04 ); /* DecoderConfigDescrTag */
        bits_fix_Decoder = bits;
        bits_write( &bits, 24,  GetDescriptorLength24b( 0 ) );
        if( p_stream->i_stream_type == 0x10 )
        {
            bits_write( &bits, 8, 0x20 );   /* Visual 14496-2 */
            bits_write( &bits, 6, 0x04 );   /* VisualStream */
        }
        else if( p_stream->i_stream_type == 0x1b )
        {
            bits_write( &bits, 8, 0x21 );   /* Visual 14496-2 */
            bits_write( &bits, 6, 0x04 );   /* VisualStream */
        }
        else if( p_stream->i_stream_type == 0x11 ||
                 p_stream->i_stream_type == 0x0f )
        {
            bits_write( &bits, 8, 0x40 );   /* Audio 14496-3 */
            bits_write( &bits, 6, 0x05 );   /* AudioStream */
        }
        else if( p_stream->i_stream_type == 0x12 &&
                 p_stream->i_codec == VLC_CODEC_SUBT )
        {
            bits_write( &bits, 8, 0x0B );   /* Text Stream */
            bits_write( &bits, 6, 0x04 );   /* VisualStream */
        }
        else
        {
            bits_write( &bits, 8, 0x00 );
            bits_write( &bits, 6, 0x00 );
            msg_Err( p_mux, "Unsupported stream_type => broken IOD" );
        }
        bits_write( &bits, 1,   0x00 );         /* UpStream */
        bits_write( &bits, 1,   0x01 );         /* reserved */
        bits_write( &bits, 24,  1024 * 1024 );  /* bufferSizeDB */
        bits_write( &bits, 32,  0x7fffffff );   /* maxBitrate */
        bits_write( &bits, 32,  0 );            /* avgBitrate */
        if( p_stream->i_extra > 0 )
        {
            /* DecoderSpecificInfo */
            bits_align( &bits );
            bits_write( &bits, 8,   0x05 ); /* tag */
            bits_write( &bits, 24, GetDescriptorLength24b(
                        p_stream->i_extra ) );
            for (int i = 0; i < p_stream->i_extra; i++ )
            {
                bits_write( &bits, 8,
                    ((uint8_t*)p_stream->p_extra)[i] );
            }
        }
        /* fix Decoder length */
        bits_write( &bits_fix_Decoder, 24,
                    GetDescriptorLength24b( bits.i_data -
                    bits_fix_Decoder.i_data - 3 ) );
        /* SLConfigDescriptor : predefined (0x01) */
        bits_align( &bits );
        bits_write( &bits, 8,   0x06 ); /* tag */
        bits_write( &bits, 24,  GetDescriptorLength24b( 8 ) );
        bits_write( &bits, 8,   0x01 );/* predefined */
        bits_write( &bits, 1,   0 );   /* durationFlag */
        bits_write( &bits, 32,  0 );   /* OCRResolution */
        bits_write( &bits, 8,   0 );   /* OCRLength */
        bits_write( &bits, 8,   0 );   /* InstantBitrateLength */
        bits_align( &bits );
        /* fix ESDescr length */
        bits_write( &bits_fix_ESDescr, 24,
                    GetDescriptorLength24b( bits.i_data -
                    bits_fix_ESDescr.i_data - 3 ) );
    }
    bits_align( &bits );
    /* fix IOD length */
    bits_write( &bits_fix_IOD, 24,
                GetDescriptorLength24b(bits.i_data - bits_fix_IOD.i_data - 3 ));
    dvbpsi_PMTAddDescriptor(&p_sys->dvbpmt[0], 0x1d, bits.i_data, bits.p_data);
}
