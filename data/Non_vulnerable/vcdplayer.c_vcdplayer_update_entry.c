static void
vcdplayer_update_entry( access_t * p_access, uint16_t ofs,
                        uint16_t *entry, const char *label)
{
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  if ( ofs == VCDINFO_INVALID_OFFSET ) {
    *entry = VCDINFO_INVALID_ENTRY;
  } else {
    vcdinfo_offset_t *off = vcdinfo_get_offset_t(p_vcdplayer->vcd, ofs);
    if (off != NULL) {
      *entry = off->lid;
      dbg_print(INPUT_DBG_PBC, "%s: LID %d", label, off->lid);
    } else
      *entry = VCDINFO_INVALID_ENTRY;
  }
}
