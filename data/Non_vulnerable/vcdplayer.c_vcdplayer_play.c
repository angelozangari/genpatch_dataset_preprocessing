void
vcdplayer_play(access_t *p_access, vcdinfo_itemid_t itemid)
{
  vcdplayer_t *p_vcdplayer = (vcdplayer_t *)p_access->p_sys;
  dbg_print(INPUT_DBG_CALL, "called itemid.num: %d itemid.type: %d",
            itemid.num, itemid.type);
  if  (!vcdplayer_pbc_is_on(p_vcdplayer)) {
    vcdplayer_play_single_item(p_access, itemid);
  } else {
    /* PBC on - Itemid.num is LID. */
    vcdinfo_obj_t *p_vcdinfo = p_vcdplayer->vcd;
    if (p_vcdinfo == NULL)
      return;
    p_vcdplayer->i_lid = itemid.num;
    vcdinfo_lid_get_pxd(p_vcdinfo, &(p_vcdplayer->pxd), itemid.num);
    switch (p_vcdplayer->pxd.descriptor_type) {
    case PSD_TYPE_SELECTION_LIST:
    case PSD_TYPE_EXT_SELECTION_LIST: {
      vcdinfo_itemid_t trans_itemid;
      uint16_t trans_itemid_num;
      if (p_vcdplayer->pxd.psd == NULL) return;
      trans_itemid_num  = vcdinf_psd_get_itemid(p_vcdplayer->pxd.psd);
      vcdinfo_classify_itemid(trans_itemid_num, &trans_itemid);
      p_vcdplayer->i_loop     = 1;
      p_vcdplayer->loop_item  = trans_itemid;
      vcdplayer_play_single_item(p_access, trans_itemid);
      break;
    }
    case PSD_TYPE_PLAY_LIST: {
      if (p_vcdplayer->pxd.pld == NULL) return;
      p_vcdplayer->pdi = -1;
      vcdplayer_inc_play_item(p_access);
      break;
    }
    case PSD_TYPE_END_LIST:
    case PSD_TYPE_COMMAND_LIST:
    default:
      ;
    }
  }
}
