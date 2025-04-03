static size_t
vcdplayer_get_item_size(access_t * p_access, vcdinfo_itemid_t itemid)
{
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  switch (itemid.type) {
  case VCDINFO_ITEM_TYPE_ENTRY:
    return p_vcdplayer->entry[itemid.num].size;
    break;
  case VCDINFO_ITEM_TYPE_SEGMENT:
    return p_vcdplayer->segment[itemid.num].size;
    break;
  case VCDINFO_ITEM_TYPE_TRACK:
    return p_vcdplayer->track[itemid.num-1].size;
    break;
  case VCDINFO_ITEM_TYPE_LID:
    /* Play list number (LID) */
    return 0;
    break;
  case VCDINFO_ITEM_TYPE_NOTFOUND:
  case VCDINFO_ITEM_TYPE_SPAREID2:
  default:
    LOG_ERR("%s %d", "bad item type", itemid.type);
    return 0;
  }
}
