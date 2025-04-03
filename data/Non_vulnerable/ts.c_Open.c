 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_mux_t          *p_mux =(sout_mux_t*)p_this;
    sout_mux_sys_t      *p_sys = NULL;
    config_ChainParse( p_mux, SOUT_CFG_PREFIX, ppsz_sout_options, p_mux->p_cfg );
    p_sys = calloc( 1, sizeof( sout_mux_sys_t ) );
    if( !p_sys )
        return VLC_ENOMEM;
    p_sys->i_num_pmt = 1;
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
    p_sys->p_dvbpsi = dvbpsi_new( &dvbpsi_messages, DVBPSI_MSG_DEBUG );
    if( !p_sys->p_dvbpsi )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_sys->p_dvbpsi->p_sys = (void *) p_mux;
#endif
    p_sys->b_es_id_pid = var_GetBool( p_mux, SOUT_CFG_PREFIX "es-id-pid" );
    /*
       fetch string of pmts. Here's a sample: --sout-ts-muxpmt="0x451,0x200,0x28a,0x240,,0x450,0x201,0x28b,0x241,,0x452,0x202,0x28c,0x242"
       This would mean 0x451, 0x200, 0x28a, 0x240 would fall under one pmt (program), 0x450,0x201,0x28b,0x241 would fall under another
    */
    char *muxpmt = var_GetNonEmptyString(p_mux, SOUT_CFG_PREFIX "muxpmt");
    for (char *psz = muxpmt; psz; )
    {
        char *psz_next;
        uint16_t i_pid = strtoul( psz, &psz_next, 0 );
        psz = *psz_next ? &psz_next[1] : NULL;
        if ( i_pid == 0 )
        {
            if ( ++p_sys->i_num_pmt > MAX_PMT )
            {
                msg_Err( p_mux, "Number of PMTs > %d)", MAX_PMT );
                p_sys->i_num_pmt = MAX_PMT;
            }
        }
        else
        {
            p_sys->pmtmap[p_sys->i_pmtslots].i_pid = i_pid;
            p_sys->pmtmap[p_sys->i_pmtslots].i_prog = p_sys->i_num_pmt - 1;
            if ( ++p_sys->i_pmtslots >= MAX_PMT_PID )
            {
                msg_Err( p_mux, "Number of pids in PMT > %d", MAX_PMT_PID );
                p_sys->i_pmtslots = MAX_PMT_PID - 1;
            }
        }
    }
    /* Now sort according to pids for fast search later on */
    qsort( (void *)p_sys->pmtmap, p_sys->i_pmtslots,
            sizeof(pmt_map_t), pmtcompare );
    free(muxpmt);
    unsigned short subi[3];
    vlc_rand_bytes(subi, sizeof(subi));
    p_sys->i_pat_version_number = nrand48(subi) & 0x1f;
    vlc_value_t val;
    var_Get( p_mux, SOUT_CFG_PREFIX "tsid", &val );
    if ( val.i_int )
        p_sys->i_tsid = val.i_int;
    else
        p_sys->i_tsid = nrand48(subi) & 0xffff;
    p_sys->i_netid = nrand48(subi) & 0xffff;
    var_Get( p_mux, SOUT_CFG_PREFIX "netid", &val );
    if ( val.i_int )
        p_sys->i_netid = val.i_int;
    p_sys->i_pmt_version_number = nrand48(subi) & 0x1f;
    p_sys->sdt.i_pid = 0x11;
    char *sdtdesc = var_GetNonEmptyString( p_mux, SOUT_CFG_PREFIX "sdtdesc" );
    /* Syntax is provider_sdt1,service_name_sdt1,provider_sdt2,service_name_sdt2... */
    if( sdtdesc )
    {
        p_sys->b_sdt = true;
        char *psz_sdttoken = sdtdesc;
        for (int i = 0; i < MAX_PMT * 2 && psz_sdttoken; i++)
        {
            sdt_desc_t *sdt = &p_sys->sdt_descriptors[i/2];
            char *psz_end = strchr( psz_sdttoken, ',' );
            if ( psz_end )
                *psz_end++ = '\0';
            if (i % 2)
                sdt->psz_service_name = strdup(psz_sdttoken);
            else
                sdt->psz_provider = strdup(psz_sdttoken);
            psz_sdttoken = psz_end;
        }
        free(sdtdesc);
    }
    p_sys->b_data_alignment = var_GetBool( p_mux, SOUT_CFG_PREFIX "alignment" );
    char *pgrpmt = var_GetNonEmptyString(p_mux, SOUT_CFG_PREFIX "program-pmt");
    if( pgrpmt )
    {
        char *psz = pgrpmt;
        char *psz_next = psz;
        for (int i = 0; psz; )
        {
            uint16_t i_pid = strtoul( psz, &psz_next, 0 );
            if( psz_next[0] != '\0' )
                psz = &psz_next[1];
            else
                psz = NULL;
            if( i_pid == 0 )
            {
                if( i >= MAX_PMT )
                    msg_Err( p_mux, "Number of PMTs > maximum (%d)", MAX_PMT );
            }
            else
            {
                p_sys->i_pmt_program_number[i] = i_pid;
                i++;
            }
        }
        free(pgrpmt);
    }
    else
    {
        /* Option not specified, use 1, 2, 3... */
        for (unsigned i = 0; i < p_sys->i_num_pmt; i++ )
            p_sys->i_pmt_program_number[i] = i + 1;
    }
    var_Get( p_mux, SOUT_CFG_PREFIX "pid-pmt", &val );
    if( !val.i_int ) /* Does this make any sense? */
        val.i_int = 0x42;
    for (unsigned i = 0; i < p_sys->i_num_pmt; i++ )
        p_sys->pmt[i].i_pid = val.i_int + i;
    p_sys->i_pid_free = p_sys->pmt[p_sys->i_num_pmt - 1].i_pid + 1;
    p_sys->i_pid_video = var_GetInteger( p_mux, SOUT_CFG_PREFIX "pid-video" );
    if ( p_sys->i_pid_video > p_sys->i_pid_free )
    {
        p_sys->i_pid_free = p_sys->i_pid_video + 1;
    }
    p_sys->i_pid_audio = var_GetInteger( p_mux, SOUT_CFG_PREFIX "pid-audio" );
    if ( p_sys->i_pid_audio > p_sys->i_pid_free )
    {
        p_sys->i_pid_free = p_sys->i_pid_audio + 1;
    }
    p_sys->i_pid_spu = var_GetInteger( p_mux, SOUT_CFG_PREFIX "pid-spu" );
    if ( p_sys->i_pid_spu > p_sys->i_pid_free )
    {
        p_sys->i_pid_free = p_sys->i_pid_spu + 1;
    }
    p_sys->i_pcr_pid = 0x1fff;
    /* Allow to create constrained stream */
    p_sys->i_bitrate_min = var_GetInteger( p_mux, SOUT_CFG_PREFIX "bmin" );
    p_sys->i_bitrate_max = var_GetInteger( p_mux, SOUT_CFG_PREFIX "bmax" );
    if( p_sys->i_bitrate_min > 0 && p_sys->i_bitrate_max > 0 &&
        p_sys->i_bitrate_min > p_sys->i_bitrate_max )
    {
        msg_Err( p_mux, "incompatible minimum and maximum bitrate, "
                 "disabling bitrate control" );
        p_sys->i_bitrate_min = 0;
        p_sys->i_bitrate_max = 0;
    }
    if( p_sys->i_bitrate_min > 0 || p_sys->i_bitrate_max > 0 )
    {
        msg_Err( p_mux, "bmin and bmax no more supported "
                 "(if you need them report it)" );
    }
    var_Get( p_mux, SOUT_CFG_PREFIX "shaping", &val );
    p_sys->i_shaping_delay = val.i_int * 1000;
    if( p_sys->i_shaping_delay <= 0 )
    {
        msg_Err( p_mux,
                 "invalid shaping (%"PRId64"ms) resetting to 200ms",
                 p_sys->i_shaping_delay / 1000 );
        p_sys->i_shaping_delay = 200000;
    }
    var_Get( p_mux, SOUT_CFG_PREFIX "pcr", &val );
    p_sys->i_pcr_delay = val.i_int * 1000;
    if( p_sys->i_pcr_delay <= 0 ||
        p_sys->i_pcr_delay >= p_sys->i_shaping_delay )
    {
        msg_Err( p_mux,
                 "invalid pcr delay (%"PRId64"ms) resetting to 70ms",
                 p_sys->i_pcr_delay / 1000 );
        p_sys->i_pcr_delay = 70000;
    }
    var_Get( p_mux, SOUT_CFG_PREFIX "dts-delay", &val );
    p_sys->i_dts_delay = val.i_int * 1000;
    msg_Dbg( p_mux, "shaping=%"PRId64" pcr=%"PRId64" dts_delay=%"PRId64,
             p_sys->i_shaping_delay, p_sys->i_pcr_delay, p_sys->i_dts_delay );
    p_sys->b_use_key_frames = var_GetBool( p_mux, SOUT_CFG_PREFIX "use-key-frames" );
    p_mux->p_sys        = p_sys;
    p_sys->csa = csaSetup(p_this);
    p_mux->pf_control   = Control;
    p_mux->pf_addstream = AddStream;
    p_mux->pf_delstream = DelStream;
    p_mux->pf_mux       = Mux;
    return VLC_SUCCESS;
}
