static void
_vcdplayer_set_entry(access_t * p_access, unsigned int num)
{
  vcdplayer_t   *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  vcdinfo_obj_t *p_vcdinfo   = p_vcdplayer->vcd;
  const unsigned int   i_entries = vcdinfo_get_num_entries(p_vcdinfo);
  if (num >= i_entries) {
    LOG_ERR("%s %d", "bad entry number", num);
    return;
  } else {
    vcdinfo_itemid_t itemid;
    itemid.num            = num;
    itemid.type           = VCDINFO_ITEM_TYPE_ENTRY;
    p_vcdplayer->i_still  = 0;
    VCDSetOrigin(p_access, vcdinfo_get_entry_lsn(p_vcdinfo, num),
        vcdinfo_get_track(p_vcdinfo, num), &itemid);
    dbg_print(INPUT_DBG_LSN, "LSN: %u, track_end LSN: %u",
              p_vcdplayer->i_lsn, p_vcdplayer->track_end_lsn);
  }
}
