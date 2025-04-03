static vcdplayer_read_status_t
vcdplayer_pbc_nav ( access_t * p_access, uint8_t *wait_time )
{
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  /* We are in playback control. */
  vcdinfo_itemid_t itemid;
  /* The end of an entry is really the end of the associated
     sequence (or track). */
  if ( (VCDINFO_ITEM_TYPE_ENTRY == p_vcdplayer->play_item.type) &&
       (p_vcdplayer->i_lsn < p_vcdplayer->end_lsn) ) {
    /* Set up to just continue to the next entry */
    p_vcdplayer->play_item.num++;
    dbg_print( (INPUT_DBG_LSN|INPUT_DBG_PBC),
               "continuing into next entry: %u", p_vcdplayer->play_item.num);
    vcdplayer_play_single_item( p_access, p_vcdplayer->play_item );
    /* p_vcdplayer->update_title(); */
    return READ_BLOCK;
  }
  switch (p_vcdplayer->pxd.descriptor_type) {
  case PSD_TYPE_END_LIST:
    return READ_END;
    break;
  case PSD_TYPE_PLAY_LIST: {
    if (vcdplayer_inc_play_item(p_access))
      return READ_BLOCK;
    /* Set up for caller process wait time given. */
    if (p_vcdplayer->i_still) {
      *wait_time = vcdinf_get_wait_time(p_vcdplayer->pxd.pld);
      dbg_print((INPUT_DBG_PBC|INPUT_DBG_STILL),
        "playlist wait time: %d", *wait_time);
      return READ_STILL_FRAME;
    }
    /* Wait time has been processed; continue with next entry. */
    vcdplayer_update_entry( p_access,
                            vcdinf_pld_get_next_offset(p_vcdplayer->pxd.pld),
                            &itemid.num, "next" );
    itemid.type = VCDINFO_ITEM_TYPE_LID;
    vcdplayer_play( p_access, itemid );
    break;
  }
  case PSD_TYPE_SELECTION_LIST:     /* Selection List (+Ext. for SVCD) */
  case PSD_TYPE_EXT_SELECTION_LIST: /* Extended Selection List (VCD2.0) */
    {
      uint16_t timeout_offs = vcdinf_get_timeout_offset(p_vcdplayer->pxd.psd);
      uint16_t max_loop     = vcdinf_get_loop_count(p_vcdplayer->pxd.psd);
      vcdinfo_offset_t *offset_timeout_LID =
        vcdinfo_get_offset_t(p_vcdplayer->vcd, timeout_offs);
      dbg_print(INPUT_DBG_PBC, "looped: %d, max_loop %d",
                p_vcdplayer->i_loop, max_loop);
      /* Set up for caller process wait time given. */
      if (p_vcdplayer->i_still) {
        *wait_time = vcdinf_get_timeout_time(p_vcdplayer->pxd.psd);
        dbg_print((INPUT_DBG_PBC|INPUT_DBG_STILL),
                  "playlist wait_time: %d", *wait_time);
        return READ_STILL_FRAME;
      }
      /* Wait time has been processed; continue with next entry. */
      /* Handle any looping given. */
      if ( max_loop == 0 || p_vcdplayer->i_loop < max_loop ) {
        p_vcdplayer->i_loop++;
        if (p_vcdplayer->i_loop == 0x7f) p_vcdplayer->i_loop = 0;
        vcdplayer_play_single_item(p_access, p_vcdplayer->loop_item);
        /* if (p_vcdplayer->i_still) p_vcdplayer->force_redisplay();*/
        return READ_BLOCK;
      }
      /* Looping finished and wait finished. Move to timeout
         entry or next entry, or handle still. */
      if (NULL != offset_timeout_LID) {
        /* Handle timeout_LID */
        itemid.num  = offset_timeout_LID->lid;
        itemid.type = VCDINFO_ITEM_TYPE_LID;
        dbg_print(INPUT_DBG_PBC, "timeout to: %d", itemid.num);
        vcdplayer_play( p_access, itemid );
        return READ_BLOCK;
      } else {
        int i_selections = vcdinf_get_num_selections(p_vcdplayer->pxd.psd);
        if (i_selections > 0) {
          /* Pick a random selection. */
          unsigned int bsn=vcdinf_get_bsn(p_vcdplayer->pxd.psd);
          int rand_selection=bsn +
            ((unsigned)vlc_lrand48() % (unsigned)i_selections);
          lid_t rand_lid=vcdinfo_selection_get_lid (p_vcdplayer->vcd,
                            p_vcdplayer->i_lid,
                            rand_selection);
          itemid.num = rand_lid;
          itemid.type = VCDINFO_ITEM_TYPE_LID;
          dbg_print(INPUT_DBG_PBC, "random selection %d, lid: %d",
                    rand_selection - bsn, rand_lid);
          vcdplayer_play( p_access, itemid );
          return READ_BLOCK;
        } else if (p_vcdplayer->i_still) {
          /* Hack: Just go back and do still again */
          msleep(10000);
          return READ_STILL_FRAME;
        }
      }
      break;
    }
  default:
    ;
  }
  /* FIXME: Should handle autowait ...  */
  return READ_ERROR;
}
