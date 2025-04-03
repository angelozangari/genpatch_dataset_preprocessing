static bool
vcdplayer_inc_play_item(access_t *p_access)
{
  vcdplayer_t *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  int noi;
  dbg_print(INPUT_DBG_CALL, "called pli: %d", p_vcdplayer->pdi);
  if ( NULL == p_vcdplayer || NULL == p_vcdplayer->pxd.pld  )
    return false;
  noi = vcdinf_pld_get_noi(p_vcdplayer->pxd.pld);
  if ( noi <= 0 )
    return false;
  /* Handle delays like autowait or wait here? */
  p_vcdplayer->pdi++;
  if ( p_vcdplayer->pdi < 0 || p_vcdplayer->pdi >= noi )
    return false;
  uint16_t trans_itemid_num=vcdinf_pld_get_play_item(p_vcdplayer->pxd.pld,
                                                     p_vcdplayer->pdi);
  vcdinfo_itemid_t trans_itemid;
  if (VCDINFO_INVALID_ITEMID == trans_itemid_num) return false;
  vcdinfo_classify_itemid(trans_itemid_num, &trans_itemid);
  dbg_print(INPUT_DBG_PBC, "  play-item[%d]: %s",
            p_vcdplayer->pdi, vcdinfo_pin2str (trans_itemid_num));
  return vcdplayer_play_single_item(p_access, trans_itemid);
}
