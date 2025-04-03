static void TSSetPCR( block_t *p_ts, mtime_t i_dts );
static csa_t *csaSetup( vlc_object_t *p_this )
{
    sout_mux_t *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    char *csack = var_CreateGetNonEmptyStringCommand( p_mux, SOUT_CFG_PREFIX "csa-ck" );
    if( !csack )
        return NULL;
    csa_t *csa = csa_New();
    if( csa_SetCW( p_this, csa, csack, true ) )
    {
        free(csack);
        csa_Delete( csa );
        return NULL;
    }
    vlc_mutex_init( &p_sys->csa_lock );
    p_sys->b_crypt_audio = var_GetBool( p_mux, SOUT_CFG_PREFIX "crypt-audio" );
    p_sys->b_crypt_video = var_GetBool( p_mux, SOUT_CFG_PREFIX "crypt-video" );
    char *csa2ck = var_CreateGetNonEmptyStringCommand( p_mux, SOUT_CFG_PREFIX "csa2-ck");
    if (!csa2ck || csa_SetCW( p_this, csa, csa2ck, false ) )
        csa_SetCW( p_this, csa, csack, false );
    free(csa2ck);
    var_Create( p_mux, SOUT_CFG_PREFIX "csa-use", VLC_VAR_STRING | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    var_AddCallback( p_mux, SOUT_CFG_PREFIX "csa-use", ActiveKeyCallback, NULL );
    var_AddCallback( p_mux, SOUT_CFG_PREFIX "csa-ck", ChangeKeyCallback, (void *)1 );
    var_AddCallback( p_mux, SOUT_CFG_PREFIX "csa2-ck", ChangeKeyCallback, NULL );
    vlc_value_t use_val;
    var_Get( p_mux, SOUT_CFG_PREFIX "csa-use", &use_val );
    if ( var_Set( p_mux, SOUT_CFG_PREFIX "csa-use", use_val ) )
        var_SetString( p_mux, SOUT_CFG_PREFIX "csa-use", "odd" );
    free( use_val.psz_string );
    p_sys->i_csa_pkt_size = var_GetInteger( p_mux, SOUT_CFG_PREFIX "csa-pkt" );
    if( p_sys->i_csa_pkt_size < 12 || p_sys->i_csa_pkt_size > 188 )
    {
        msg_Err( p_mux, "wrong packet size %d specified",
            p_sys->i_csa_pkt_size );
        p_sys->i_csa_pkt_size = 188;
    }
    msg_Dbg( p_mux, "encrypting %d bytes of packet", p_sys->i_csa_pkt_size );
    free(csack);
    return csa;
}
