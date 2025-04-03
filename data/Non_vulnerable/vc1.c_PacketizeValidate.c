}
static int PacketizeValidate( void *p_private, block_t *p_au )
{
    decoder_t *p_dec = p_private;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->i_interpolated_dts <= VLC_TS_INVALID )
    {
        msg_Dbg( p_dec, "need a starting pts/dts" );
        return VLC_EGENERIC;
    }
    VLC_UNUSED(p_au);
    return VLC_SUCCESS;
}
