 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_mux_t          *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t      *p_sys = p_mux->p_sys;
#if (DVBPSI_VERSION_INT >= DVBPSI_VERSION_WANTED(1,0,0))
    if( p_sys->p_dvbpsi )
        dvbpsi_delete( p_sys->p_dvbpsi );
#endif
    if( p_sys->csa )
    {
        var_DelCallback( p_mux, SOUT_CFG_PREFIX "csa-ck", ChangeKeyCallback, NULL );
        var_DelCallback( p_mux, SOUT_CFG_PREFIX "csa2-ck", ChangeKeyCallback, NULL );
        var_DelCallback( p_mux, SOUT_CFG_PREFIX "csa-use", ActiveKeyCallback, NULL );
        csa_Delete( p_sys->csa );
        vlc_mutex_destroy( &p_sys->csa_lock );
    }
    for (int i = 0; i < MAX_PMT; i++ )
    {
        free( p_sys->sdt_descriptors[i].psz_service_name );
        free( p_sys->sdt_descriptors[i].psz_provider );
    }
    free( p_sys->dvbpmt );
    free( p_sys );
}
