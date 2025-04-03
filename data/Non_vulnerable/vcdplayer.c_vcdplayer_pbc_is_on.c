bool
vcdplayer_pbc_is_on( const vcdplayer_t *p_vcdplayer )
{
  return VCDINFO_INVALID_ENTRY != p_vcdplayer->i_lid;
}
