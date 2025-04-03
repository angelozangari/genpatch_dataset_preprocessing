}
static void GetPMT( sout_mux_t *p_mux, sout_buffer_chain_t *c )
{
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    if( p_sys->dvbpmt == NULL )
    {
        p_sys->dvbpmt = malloc( p_sys->i_num_pmt * sizeof(dvbpsi_pmt_t) );
        if( p_sys->dvbpmt == NULL )
            return;
    }
    dvbpsi_sdt_t sdt;
    if( p_sys->b_sdt )
        dvbpsi_InitSDT( &sdt, p_sys->i_tsid, 1, 1, p_sys->i_netid );
    for (unsigned i = 0; i < p_sys->i_num_pmt; i++ )
    {
        dvbpsi_InitPMT( &p_sys->dvbpmt[i],
                        p_sys->i_pmt_program_number[i],   /* program number */
                        p_sys->i_pmt_version_number,
                        1,      /* b_current_next */
                        p_sys->i_pcr_pid );
        if( !p_sys->b_sdt )
            continue;
        dvbpsi_sdt_service_t *p_service = dvbpsi_SDTAddService( &sdt,
            p_sys->i_pmt_program_number[i],  /* service id */
            0,         /* eit schedule */
            0,         /* eit present */
            4,         /* running status ("4=RUNNING") */
            0 );       /* free ca */
        const char *psz_sdtprov = p_sys->sdt_descriptors[i].psz_provider;
        const char *psz_sdtserv = p_sys->sdt_descriptors[i].psz_service_name;
        if( !psz_sdtprov || !psz_sdtserv )
            continue;
        size_t provlen = VLC_CLIP(strlen(psz_sdtprov), 0, 255);
        size_t servlen = VLC_CLIP(strlen(psz_sdtserv), 0, 255);
        uint8_t psz_sdt_desc[3 + provlen + servlen];
        psz_sdt_desc[0] = 0x01; /* digital television service */
        /* service provider name length */
        psz_sdt_desc[1] = (char)provlen;
        memcpy( &psz_sdt_desc[2], psz_sdtprov, provlen );
        /* service name length */
        psz_sdt_desc[ 2 + provlen ] = (char)servlen;
        memcpy( &psz_sdt_desc[3+provlen], psz_sdtserv, servlen );
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
        dvbpsi_sdt_service_descriptor_add( p_service, 0x48,
                                           (3 + provlen + servlen),
                                           psz_sdt_desc );
#else
        dvbpsi_SDTServiceAddDescriptor( p_service, 0x48,
                3 + provlen + servlen, psz_sdt_desc );
#endif
    }
    if( p_sys->i_mpeg4_streams > 0 )
        GetPMTmpeg4(p_mux);
    for (int i_stream = 0; i_stream < p_mux->i_nb_inputs; i_stream++ )
    {
        ts_stream_t *p_stream = (ts_stream_t*)p_mux->pp_inputs[i_stream]->p_sys;
        int i_pidinput = p_mux->pp_inputs[i_stream]->p_fmt->i_id;
        pmt_map_t *p_usepid = bsearch( &i_pidinput, p_sys->pmtmap,
                    p_sys->i_pmtslots, sizeof(pmt_map_t), intcompare );
        /* If there's an error somewhere, dump it to the first pmt */
        unsigned prog = p_usepid ? p_usepid->i_prog : 0;
        dvbpsi_pmt_es_t *p_es = dvbpsi_PMTAddES( &p_sys->dvbpmt[prog],
                    p_stream->i_stream_type, p_stream->i_pid );
        if( p_stream->i_stream_id == 0xfa || p_stream->i_stream_id == 0xfb )
        {
            uint8_t     es_id[2];
            /* SL descriptor */
            es_id[0] = (p_stream->i_es_id >> 8)&0xff;
            es_id[1] = (p_stream->i_es_id)&0xff;
            dvbpsi_PMTESAddDescriptor( p_es, 0x1f, 2, es_id );
        }
        else if( p_stream->i_stream_type == 0xa0 )
        {
            uint8_t data[512];
            int i_extra = __MIN( p_stream->i_extra, 502 );
            /* private DIV3 descripor */
            memcpy( &data[0], &p_stream->i_bih_codec, 4 );
            data[4] = ( p_stream->i_bih_width >> 8 )&0xff;
            data[5] = ( p_stream->i_bih_width      )&0xff;
            data[6] = ( p_stream->i_bih_height>> 8 )&0xff;
            data[7] = ( p_stream->i_bih_height     )&0xff;
            data[8] = ( i_extra >> 8 )&0xff;
            data[9] = ( i_extra      )&0xff;
            if( i_extra > 0 )
            {
                memcpy( &data[10], p_stream->p_extra, i_extra );
            }
            /* 0xa0 is private */
            dvbpsi_PMTESAddDescriptor( p_es, 0xa0, i_extra + 10, data );
        }
        else if( p_stream->i_stream_type == 0x81 )
        {
            uint8_t format[4] = { 'A', 'C', '-', '3'};
            /* "registration" descriptor : "AC-3" */
            dvbpsi_PMTESAddDescriptor( p_es, 0x05, 4, format );
        }
        else if( p_stream->i_codec == VLC_CODEC_DIRAC )
        {
            /* Dirac registration descriptor */
            uint8_t data[4] = { 'd', 'r', 'a', 'c' };
            dvbpsi_PMTESAddDescriptor( p_es, 0x05, 4, data );
        }
        else if( p_stream->i_codec == VLC_CODEC_DTS )
        {
            /* DTS registration descriptor (ETSI TS 101 154 Annex F) */
            /* DTS format identifier, frame size 1024 - FIXME */
            uint8_t data[4] = { 'D', 'T', 'S', '2' };
            dvbpsi_PMTESAddDescriptor( p_es, 0x05, 4, data );
        }
        else if( p_stream->i_codec == VLC_CODEC_EAC3 )
        {
            uint8_t data[1] = { 0x00 };
            dvbpsi_PMTESAddDescriptor( p_es, 0x7a, 1, data );
        }
        else if( p_stream->i_codec == VLC_CODEC_TELETEXT )
        {
            if( p_stream->i_extra )
            {
                dvbpsi_PMTESAddDescriptor( p_es, 0x56,
                                           p_stream->i_extra,
                                           p_stream->p_extra );
            }
            continue;
        }
        else if( p_stream->i_codec == VLC_CODEC_DVBS )
        {
            /* DVB subtitles */
            if( p_stream->i_extra )
            {
                /* pass-through from the TS demux */
                dvbpsi_PMTESAddDescriptor( p_es, 0x59,
                                           p_stream->i_extra,
                                           p_stream->p_extra );
            }
            else
            {
                /* from the dvbsub transcoder */
                dvbpsi_subtitling_dr_t descr;
                dvbpsi_subtitle_t sub;
                dvbpsi_descriptor_t *p_descr;
                memcpy( sub.i_iso6392_language_code, p_stream->lang, 3 );
                sub.i_subtitling_type = 0x10; /* no aspect-ratio criticality */
                sub.i_composition_page_id = p_stream->i_es_id & 0xFF;
                sub.i_ancillary_page_id = p_stream->i_es_id >> 16;
                descr.i_subtitles_number = 1;
                descr.p_subtitle[0] = sub;
                p_descr = dvbpsi_GenSubtitlingDr( &descr, 0 );
                /* Work around bug in old libdvbpsi */ p_descr->i_length = 8;
                dvbpsi_PMTESAddDescriptor( p_es, p_descr->i_tag,
                                           p_descr->i_length, p_descr->p_data );
            }
            continue;
        }
        if( p_stream->i_langs )
        {
            dvbpsi_PMTESAddDescriptor( p_es, 0x0a, 4*p_stream->i_langs,
                p_stream->lang);
        }
    }
    for (unsigned i = 0; i < p_sys->i_num_pmt; i++ )
    {
        dvbpsi_psi_section_t *sect;
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
        sect = dvbpsi_pmt_sections_generate( p_sys->p_dvbpsi, &p_sys->dvbpmt[i] );
#else
        sect = dvbpsi_GenPMTSections( &p_sys->dvbpmt[i] );
#endif
        block_t *pmt = WritePSISection( sect );
        PEStoTS( c, pmt, &p_sys->pmt[i] );
        dvbpsi_DeletePSISections(sect);
        dvbpsi_EmptyPMT( &p_sys->dvbpmt[i] );
    }
    if( p_sys->b_sdt )
    {
        dvbpsi_psi_section_t *sect;
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
        sect = dvbpsi_sdt_sections_generate( p_sys->p_dvbpsi, &sdt );
#else
        sect = dvbpsi_GenSDTSections( &sdt );
#endif
        block_t *p_sdt = WritePSISection( sect );
        PEStoTS( c, p_sdt, &p_sys->sdt );
        dvbpsi_DeletePSISections( sect );
        dvbpsi_EmptySDT( &sdt );
    }
}
