bool
vcdplayer_play_next( access_t * p_access )
{
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  vcdinfo_obj_t     *p_vcdinfo;
  vcdinfo_itemid_t   itemid;
  if (!p_vcdplayer) return false;
  dbg_print( (INPUT_DBG_CALL|INPUT_DBG_PBC),
         "current: %d" , p_vcdplayer->play_item.num);
  p_vcdinfo = p_vcdplayer->vcd;
  itemid = p_vcdplayer->play_item;
  if  (vcdplayer_pbc_is_on(p_vcdplayer)) {
    vcdinfo_lid_get_pxd(p_vcdinfo, &(p_vcdplayer->pxd), p_vcdplayer->i_lid);
    switch (p_vcdplayer->pxd.descriptor_type) {
    case PSD_TYPE_SELECTION_LIST:
    case PSD_TYPE_EXT_SELECTION_LIST:
      if (p_vcdplayer->pxd.psd == NULL) return false;
      vcdplayer_update_entry(p_access,
                             vcdinf_psd_get_next_offset(p_vcdplayer->pxd.psd),
                             &itemid.num, "next");
      itemid.type = VCDINFO_ITEM_TYPE_LID;
      break;
    case PSD_TYPE_PLAY_LIST:
      if (p_vcdplayer->pxd.pld == NULL) return false;
      vcdplayer_update_entry(p_access,
                             vcdinf_pld_get_next_offset(p_vcdplayer->pxd.pld),
                             &itemid.num, "next");
      itemid.type = VCDINFO_ITEM_TYPE_LID;
      break;
    case PSD_TYPE_END_LIST:
    case PSD_TYPE_COMMAND_LIST:
      LOG_WARN( "There is no PBC 'next' selection here" );
      return false;
    }
  } else {
    /* PBC is not on. "Next" selection is play_item.num+1 if possible. */
    int max_entry = 0;
    switch (p_vcdplayer->play_item.type) {
    case VCDINFO_ITEM_TYPE_ENTRY:
    case VCDINFO_ITEM_TYPE_SEGMENT:
    case VCDINFO_ITEM_TYPE_TRACK:
      switch (p_vcdplayer->play_item.type) {
      case VCDINFO_ITEM_TYPE_ENTRY:
        max_entry = p_vcdplayer->i_entries;
        break;
      case VCDINFO_ITEM_TYPE_SEGMENT:
        max_entry = p_vcdplayer->i_segments;
        break;
      case VCDINFO_ITEM_TYPE_TRACK:
        max_entry = p_vcdplayer->i_tracks;
        break;
      default: ; /* Handle exceptional cases below */
      }
      if (p_vcdplayer->play_item.num+1 < max_entry) {
        itemid.num = p_vcdplayer->play_item.num+1;
      } else {
        LOG_WARN( "At the end - non-PBC 'next' not possible here" );
        return false;
      }
      break;
    case VCDINFO_ITEM_TYPE_LID:
      /* Should have handled above. */
      LOG_WARN( "Internal inconsistency - should not have gotten here." );
      return false;
    default:
      return false;
    }
  }
  /** ??? p_vcdplayer->update_title(); ***/
  vcdplayer_play( p_access, itemid );
  return VLC_SUCCESS;
}
