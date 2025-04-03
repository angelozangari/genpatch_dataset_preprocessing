}
static void GetPAT( sout_mux_t *p_mux,
                    sout_buffer_chain_t *c )
{
    sout_mux_sys_t       *p_sys = p_mux->p_sys;
    block_t              *p_pat;
    dvbpsi_pat_t         pat;
    dvbpsi_psi_section_t *p_section;
    dvbpsi_InitPAT( &pat, p_sys->i_tsid, p_sys->i_pat_version_number,
                    1 );      /* b_current_next */
    /* add all programs */
    for (unsigned i = 0; i < p_sys->i_num_pmt; i++ )
        dvbpsi_PATAddProgram( &pat, p_sys->i_pmt_program_number[i],
                              p_sys->pmt[i].i_pid );
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
    p_section = dvbpsi_pat_sections_generate( p_sys->p_dvbpsi, &pat, 0 );
#else
    p_section = dvbpsi_GenPATSections( &pat, 0 /* max program per section */ );
#endif
    p_pat = WritePSISection( p_section );
    PEStoTS( c, p_pat, &p_sys->pat );
    dvbpsi_DeletePSISections( p_section );
    dvbpsi_EmptyPAT( &pat );
}
