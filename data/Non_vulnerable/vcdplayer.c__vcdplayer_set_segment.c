static void
_vcdplayer_set_segment(access_t * p_access, unsigned int num)
{
  vcdplayer_t   *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  vcdinfo_obj_t *p_vcdinfo   = p_vcdplayer->vcd;
  segnum_t       i_segs    = vcdinfo_get_num_segments(p_vcdinfo);
  if (num >= i_segs) {
    LOG_ERR("%s %d", "bad segment number", num);
    return;
  } else {
    vcdinfo_itemid_t itemid;
    if (VCDINFO_NULL_LSN==p_vcdplayer->i_lsn) {
      LOG_ERR("%s %d",
              "Error in getting current segment number", num);
      return;
    }
    itemid.num = num;
    itemid.type = VCDINFO_ITEM_TYPE_SEGMENT;
    VCDSetOrigin(p_access, vcdinfo_get_seg_lsn(p_vcdinfo, num), 0, &itemid);
    dbg_print(INPUT_DBG_LSN, "LSN: %u", p_vcdplayer->i_lsn);
  }
}
