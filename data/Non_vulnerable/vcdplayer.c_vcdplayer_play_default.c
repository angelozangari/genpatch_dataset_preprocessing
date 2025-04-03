bool
vcdplayer_play_default( access_t * p_access )
{
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  vcdinfo_itemid_t itemid;
  if (!p_vcdplayer) {
    dbg_print( (INPUT_DBG_CALL|INPUT_DBG_PBC),
           "null p_vcdplayer" );
    return VLC_EGENERIC;
  }
  dbg_print( (INPUT_DBG_CALL|INPUT_DBG_PBC),
         "current: %d" , p_vcdplayer->play_item.num);
  itemid.type = p_vcdplayer->play_item.type;
  if (vcdplayer_pbc_is_on(p_vcdplayer)) {
#if defined(LIBVCD_VERSION)
    lid_t lid=vcdinfo_get_multi_default_lid(p_vcdplayer->vcd, p_vcdplayer->i_lid,
                        p_vcdplayer->i_lsn);
    if (VCDINFO_INVALID_LID != lid) {
      itemid.num  = lid;
      itemid.type = VCDINFO_ITEM_TYPE_LID;
      dbg_print(INPUT_DBG_PBC, "DEFAULT to %d", itemid.num);
    } else {
      dbg_print(INPUT_DBG_PBC, "no DEFAULT for LID %d", p_vcdplayer->i_lid);
      return VLC_EGENERIC;
    }
#else
    vcdinfo_lid_get_pxd(p_vcdplayer->vcd, &(p_vcdplayer->pxd),
                        p_vcdplayer->i_lid);
    switch (p_vcdplayer->pxd.descriptor_type) {
    case PSD_TYPE_SELECTION_LIST:
    case PSD_TYPE_EXT_SELECTION_LIST:
      if (p_vcdplayer->pxd.psd == NULL) return false;
      vcdplayer_update_entry(p_access,
                             vcdinfo_get_default_offset(p_vcdplayer->vcd,
                                                        p_vcdplayer->i_lid),
                             &itemid.num, "default");
      break;
    case PSD_TYPE_PLAY_LIST:
    case PSD_TYPE_END_LIST:
    case PSD_TYPE_COMMAND_LIST:
      LOG_WARN( "There is no PBC 'default' selection here" );
      return false;
    }
#endif /* LIBVCD_VERSION (< 0.7.21) */
  } else {
    /* PBC is not on. "default" selection beginning of current
       selection . */
    itemid.num = p_vcdplayer->play_item.num;
  }
  /** ??? p_vcdplayer->update_title(); ***/
  vcdplayer_play( p_access, itemid );
  return VLC_SUCCESS;
}
