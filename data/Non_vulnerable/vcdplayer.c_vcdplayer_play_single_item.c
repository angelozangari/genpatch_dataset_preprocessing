static bool
vcdplayer_play_single_item( access_t * p_access, vcdinfo_itemid_t itemid)
{
  vcdplayer_t   *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  vcdinfo_obj_t *p_vcdinfo = p_vcdplayer->vcd;
  dbg_print(INPUT_DBG_CALL, "called itemid.num: %d, itemid.type: %d",
            itemid.num, itemid.type);
  p_vcdplayer->i_still = 0;
  switch (itemid.type) {
  case VCDINFO_ITEM_TYPE_SEGMENT:
    {
      vcdinfo_video_segment_type_t segtype
        = vcdinfo_get_video_type(p_vcdinfo, itemid.num);
      segnum_t i_segs = vcdinfo_get_num_segments(p_vcdinfo);
      dbg_print(INPUT_DBG_PBC, "%s (%d), itemid.num: %d",
                vcdinfo_video_type2str(p_vcdinfo, itemid.num),
                (int) segtype, itemid.num);
      if (itemid.num >= i_segs) return false;
      _vcdplayer_set_segment(p_access, itemid.num);
      switch (segtype)
        {
        case VCDINFO_FILES_VIDEO_NTSC_STILL:
        case VCDINFO_FILES_VIDEO_NTSC_STILL2:
        case VCDINFO_FILES_VIDEO_PAL_STILL:
        case VCDINFO_FILES_VIDEO_PAL_STILL2:
          p_vcdplayer->i_still = STILL_READING;
          break;
        default:
          p_vcdplayer->i_still = 0;
        }
      break;
    }
  case VCDINFO_ITEM_TYPE_TRACK:
    dbg_print(INPUT_DBG_PBC, "track %d", itemid.num);
    if (itemid.num < 1 || itemid.num > p_vcdplayer->i_tracks) return false;
    _vcdplayer_set_track(p_access, itemid.num);
    break;
  case VCDINFO_ITEM_TYPE_ENTRY:
    {
      unsigned int i_entries = vcdinfo_get_num_entries(p_vcdinfo);
      dbg_print(INPUT_DBG_PBC, "entry %d", itemid.num);
      if (itemid.num >= i_entries) return false;
      _vcdplayer_set_entry(p_access, itemid.num);
      break;
    }
  case VCDINFO_ITEM_TYPE_LID:
    LOG_ERR("%s", "Should have converted p_vcdplayer above");
    return false;
    break;
  case VCDINFO_ITEM_TYPE_NOTFOUND:
    dbg_print(INPUT_DBG_PBC, "play nothing");
    p_vcdplayer->i_lsn = p_vcdplayer->end_lsn;
    return false;
  default:
    LOG_ERR("item type %d not implemented.", itemid.type);
    return false;
  }
  p_vcdplayer->play_item = itemid;
  /* Some players like xine, have a fifo queue of audio and video buffers
     that need to be flushed when playing a new selection. */
  /*  if (p_vcdplayer->flush_buffers)
      p_vcdplayer->flush_buffers(); */
  return true;
}
