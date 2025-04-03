static void
_vcdplayer_set_track(access_t * p_access, track_t i_track)
{
  vcdplayer_t *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  if (i_track < 1 || i_track > p_vcdplayer->i_tracks)
    return;
  else {
    const vcdinfo_obj_t *p_vcdinfo = p_vcdplayer->vcd;
    vcdinfo_itemid_t itemid;
    itemid.num             = i_track;
    itemid.type            = VCDINFO_ITEM_TYPE_TRACK;
    p_vcdplayer->in_still  = 0;
    VCDSetOrigin(p_access, vcdinfo_get_track_lsn(p_vcdinfo, i_track),
         i_track, &itemid);
    dbg_print(INPUT_DBG_LSN, "LSN: %u", p_vcdplayer->i_lsn);
  }
}
