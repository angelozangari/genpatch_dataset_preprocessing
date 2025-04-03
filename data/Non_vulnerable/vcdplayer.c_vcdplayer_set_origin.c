void
vcdplayer_set_origin(access_t *p_access, lsn_t i_lsn, track_t i_track,
             const vcdinfo_itemid_t *p_itemid)
{
  vcdplayer_t *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  const size_t i_size= vcdplayer_get_item_size(p_access, *p_itemid);
  if( VCDINFO_NULL_LSN == i_lsn )
  {
    LOG_ERR("%s %d", "Invalid LSN for track", i_track);
    return;
  }
  p_vcdplayer->play_item.num  = p_itemid->num;
  p_vcdplayer->play_item.type = p_itemid->type;
  p_vcdplayer->i_lsn          = i_lsn;
  p_vcdplayer->end_lsn        = p_vcdplayer->i_lsn + i_size;
  p_vcdplayer->origin_lsn     = p_vcdplayer->i_lsn;
  p_vcdplayer->i_track        = i_track;
  p_vcdplayer->track_lsn      = vcdinfo_get_track_lsn(p_vcdplayer->vcd,
                              i_track);
  p_vcdplayer->track_end_lsn  = p_vcdplayer->track_lsn +
    vcdinfo_get_track_sect_count(p_vcdplayer->vcd, i_track);
  dbg_print((INPUT_DBG_CALL|INPUT_DBG_LSN),
        "lsn %u, end LSN: %u item.num %d, item.type %d",
        p_vcdplayer->i_lsn, p_vcdplayer->end_lsn,
        p_vcdplayer->play_item.num, p_vcdplayer->play_item.type);
}
