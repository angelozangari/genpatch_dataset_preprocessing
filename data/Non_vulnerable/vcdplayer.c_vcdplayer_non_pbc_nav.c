static vcdplayer_read_status_t
vcdplayer_non_pbc_nav ( access_t *p_access, uint8_t *wait_time )
{
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  /* Not in playback control. Do we advance automatically or stop? */
  switch (p_vcdplayer->play_item.type) {
  case VCDINFO_ITEM_TYPE_TRACK:
  case VCDINFO_ITEM_TYPE_ENTRY: {
    if ( ! vcdplayer_play_next( p_access ) )
      return READ_END;
    break;
  }
  case VCDINFO_ITEM_TYPE_SPAREID2:
    dbg_print( (INPUT_DBG_STILL|INPUT_DBG_LSN),
               "SPAREID2" );
    if (p_vcdplayer->in_still)
    {
      dbg_print( (INPUT_DBG_STILL|INPUT_DBG_LSN),
                 "End of still spareid2" );
      *wait_time = 255;
      return READ_STILL_FRAME ;
    }
    return READ_END;
  case VCDINFO_ITEM_TYPE_NOTFOUND:
    LOG_ERR ("NOTFOUND outside PBC -- not supposed to happen");
    return READ_ERROR;
  case VCDINFO_ITEM_TYPE_LID:
    LOG_ERR ("LID outside PBC -- not supposed to happen");
    return READ_ERROR;
  case VCDINFO_ITEM_TYPE_SEGMENT:
      /* Hack: Just go back and do still again */
    /* FIXME */
    if (p_vcdplayer->in_still)
    {
      dbg_print( (INPUT_DBG_STILL|INPUT_DBG_LSN),
                 "End of still Segment" );
      *wait_time = 10;
      return READ_STILL_FRAME;
    }
    return READ_END;
  }
  return READ_BLOCK;
}
