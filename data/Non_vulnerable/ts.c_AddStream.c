 *****************************************************************************/
static int AddStream( sout_mux_t *p_mux, sout_input_t *p_input )
{
    sout_mux_sys_t      *p_sys = p_mux->p_sys;
    ts_stream_t         *p_stream;
    p_input->p_sys = p_stream = calloc( 1, sizeof( ts_stream_t ) );
    if( !p_stream )
        goto oom;
    if ( p_sys->b_es_id_pid )
        p_stream->i_pid = p_input->p_fmt->i_id & 0x1fff;
    else
        p_stream->i_pid = AllocatePID( p_sys, p_input->p_fmt->i_cat );
    p_stream->i_codec = p_input->p_fmt->i_codec;
    p_stream->i_stream_type = -1;
    switch( p_input->p_fmt->i_codec )
    {
    /* VIDEO */
    case VLC_CODEC_MPGV:
    case VLC_CODEC_MP2V:
    case VLC_CODEC_MP1V:
        /* TODO: do we need to check MPEG-I/II ? */
        p_stream->i_stream_type = 0x02;
        p_stream->i_stream_id = 0xe0;
        break;
    case VLC_CODEC_MP4V:
        p_stream->i_stream_type = 0x10;
        p_stream->i_stream_id = 0xe0;
        p_stream->i_es_id = p_stream->i_pid;
        break;
    case VLC_CODEC_HEVC:
        p_stream->i_stream_type = 0x24;
        p_stream->i_stream_id = 0xe0;
        break;
    case VLC_CODEC_H264:
        p_stream->i_stream_type = 0x1b;
        p_stream->i_stream_id = 0xe0;
        break;
    /* XXX dirty dirty but somebody want crapy MS-codec XXX */
    case VLC_CODEC_H263I:
    case VLC_CODEC_H263:
    case VLC_CODEC_WMV3:
    case VLC_CODEC_WMV2:
    case VLC_CODEC_WMV1:
    case VLC_CODEC_DIV3:
    case VLC_CODEC_DIV2:
    case VLC_CODEC_DIV1:
    case VLC_CODEC_MJPG:
        p_stream->i_stream_type = 0xa0; /* private */
        p_stream->i_stream_id = 0xa0;   /* beurk */
        p_stream->i_bih_codec  = p_input->p_fmt->i_codec;
        p_stream->i_bih_width  = p_input->p_fmt->video.i_width;
        p_stream->i_bih_height = p_input->p_fmt->video.i_height;
        break;
    case VLC_CODEC_DIRAC:
        /* stream_id makes use of stream_id_extension */
        p_stream->i_stream_id = (PES_EXTENDED_STREAM_ID << 8) | 0x60;
        p_stream->i_stream_type = 0xd1;
        break;
    /* AUDIO */
    case VLC_CODEC_MPGA:
    case VLC_CODEC_MP3:
        p_stream->i_stream_type =
            p_input->p_fmt->audio.i_rate >= 32000 ? 0x03 : 0x04;
        p_stream->i_stream_id = 0xc0;
        break;
    case VLC_CODEC_A52:
        p_stream->i_stream_type = 0x81;
        p_stream->i_stream_id = 0xbd;
        break;
    case VLC_CODEC_EAC3:
        p_stream->i_stream_type = 0x06;
        p_stream->i_stream_id = 0xbd;
        break;
    case VLC_CODEC_DVD_LPCM:
        p_stream->i_stream_type = 0x83;
        p_stream->i_stream_id = 0xbd;
        break;
    case VLC_CODEC_DTS:
        p_stream->i_stream_type = 0x06;
        p_stream->i_stream_id = 0xbd;
        break;
    case VLC_CODEC_MP4A:
        /* XXX: make that configurable in some way when LOAS
         * is implemented for AAC in TS */
        //p_stream->i_stream_type = 0x11; /* LOAS/LATM */
        p_stream->i_stream_type = 0x0f; /* ADTS */
        p_stream->i_stream_id = 0xc0;
        p_stream->i_es_id = p_stream->i_pid;
        break;
    /* TEXT */
    case VLC_CODEC_SPU:
        p_stream->i_stream_type = 0x82;
        p_stream->i_stream_id = 0xbd;
        break;
    case VLC_CODEC_SUBT:
        p_stream->i_stream_type = 0x12;
        p_stream->i_stream_id = 0xfa;
        p_sys->i_mpeg4_streams++;
        p_stream->i_es_id = p_stream->i_pid;
        break;
    case VLC_CODEC_DVBS:
        p_stream->i_stream_type = 0x06;
        p_stream->i_es_id = p_input->p_fmt->subs.dvb.i_id;
        p_stream->i_stream_id = 0xbd;
        break;
    case VLC_CODEC_TELETEXT:
        p_stream->i_stream_type = 0x06;
        p_stream->i_stream_id = 0xbd; /* FIXME */
        break;
    }
    if (p_stream->i_stream_type == -1)
    {
        msg_Warn( p_mux, "rejecting stream with unsupported codec %4.4s",
                  (char*)&p_stream->i_codec );
        free( p_stream );
        return VLC_EGENERIC;
    }
    p_stream->i_langs = 1 + p_input->p_fmt->i_extra_languages;
    p_stream->lang = calloc(1, p_stream->i_langs * 4);
    if( !p_stream->lang )
        goto oom;
    msg_Dbg( p_mux, "adding input codec=%4.4s pid=%d",
             (char*)&p_stream->i_codec, p_stream->i_pid );
    for (int i = 0; i < p_stream->i_langs; i++) {
        char *lang = (i == 0)
            ? p_input->p_fmt->psz_language
            : p_input->p_fmt->p_extra_languages[i-1].psz_language;
        if (!lang)
            continue;
        const char *code = GetIso639_2LangCode(lang);
        if (*code)
        {
            memcpy(&p_stream->lang[i*4], code, 3);
            p_stream->lang[i*4+3] = 0x00; /* audio type: 0x00 undefined */
            msg_Dbg( p_mux, "    - lang=%3.3s", &p_stream->lang[i*4] );
        }
    }
    /* Create decoder specific info for subt */
    if( p_stream->i_codec == VLC_CODEC_SUBT )
    {
        p_stream->i_extra = 55;
        p_stream->p_extra = malloc( p_stream->i_extra );
        if (!p_stream->p_extra)
            goto oom;
        uint8_t *p = p_stream->p_extra;
        p[0] = 0x10;    /* textFormat, 0x10 for 3GPP TS 26.245 */
        p[1] = 0x00;    /* flags: 1b: associated video info flag
                                3b: reserved
                                1b: duration flag
                                3b: reserved */
        p[2] = 52;      /* remaining size */
        p += 3;
        p[0] = p[1] = p[2] = p[3] = 0; p+=4;    /* display flags */
        *p++ = 0;  /* horizontal justification (-1: left, 0 center, 1 right) */
        *p++ = 1;  /* vertical   justification (-1: top, 0 center, 1 bottom) */
        p[0] = p[1] = p[2] = 0x00; p+=3;/* background rgb */
        *p++ = 0xff;                    /* background a */
        p[0] = p[1] = 0; p += 2;        /* text box top */
        p[0] = p[1] = 0; p += 2;        /* text box left */
        p[0] = p[1] = 0; p += 2;        /* text box bottom */
        p[0] = p[1] = 0; p += 2;        /* text box right */
        p[0] = p[1] = 0; p += 2;        /* start char */
        p[0] = p[1] = 0; p += 2;        /* end char */
        p[0] = p[1] = 0; p += 2;        /* default font id */
        *p++ = 0;                       /* font style flags */
        *p++ = 12;                      /* font size */
        p[0] = p[1] = p[2] = 0x00; p+=3;/* foreground rgb */
        *p++ = 0x00;                    /* foreground a */
        p[0] = p[1] = p[2] = 0; p[3] = 22; p += 4;
        memcpy( p, "ftab", 4 ); p += 4;
        *p++ = 0; *p++ = 1;             /* entry count */
        p[0] = p[1] = 0; p += 2;        /* font id */
        *p++ = 9;                       /* font name length */
        memcpy( p, "Helvetica", 9 );    /* font name */
    }
    else
    {
        /* Copy extra data (VOL for MPEG-4 and extra BitMapInfoHeader for VFW */
        es_format_t *fmt = p_input->p_fmt;
        if( fmt->i_extra > 0 )
        {
            p_stream->i_extra = fmt->i_extra;
            p_stream->p_extra = malloc( fmt->i_extra );
            if( !p_stream->p_extra )
                goto oom;
            memcpy( p_stream->p_extra, fmt->p_extra, fmt->i_extra );
        }
    }
    /* Init pes chain */
    BufferChainInit( &p_stream->chain_pes );
    /* We only change PMT version (PAT isn't changed) */
    p_sys->i_pmt_version_number = ( p_sys->i_pmt_version_number + 1 )%32;
    /* Update pcr_pid */
    if( p_input->p_fmt->i_cat != SPU_ES &&
        ( p_sys->i_pcr_pid == 0x1fff || p_input->p_fmt->i_cat == VIDEO_ES ) )
    {
        if( p_sys->p_pcr_input )
        {
            /* There was already a PCR stream, so clean context */
            /* FIXME */
        }
        p_sys->i_pcr_pid   = p_stream->i_pid;
        p_sys->p_pcr_input = p_input;
        msg_Dbg( p_mux, "new PCR PID is %d", p_sys->i_pcr_pid );
    }
    return VLC_SUCCESS;
oom:
    if(p_stream)
    {
        free(p_stream->lang);
        free(p_stream);
    }
    return VLC_ENOMEM;
}
