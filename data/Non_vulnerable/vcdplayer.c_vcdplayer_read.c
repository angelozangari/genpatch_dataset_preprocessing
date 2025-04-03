vcdplayer_read_status_t
vcdplayer_read (access_t * p_access, uint8_t *p_buf)
{
  /* p_access->handle_events (); */
  uint8_t wait_time=0;
  vcdplayer_t *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  if ( p_vcdplayer->i_lsn > p_vcdplayer->end_lsn ) {
    vcdplayer_read_status_t read_status;
    /* We've run off of the end of this entry. Do we continue or stop? */
    dbg_print( (INPUT_DBG_LSN|INPUT_DBG_PBC),
              "end reached, cur: %u, end: %u\n",
              p_vcdplayer->i_lsn, p_vcdplayer->end_lsn);
  handle_item_continuation:
    read_status = vcdplayer_pbc_is_on( p_vcdplayer )
      ? vcdplayer_pbc_nav( p_access, &wait_time )
      : vcdplayer_non_pbc_nav( p_access, &wait_time );
    if (READ_STILL_FRAME == read_status) {
      *p_buf = wait_time;
      return READ_STILL_FRAME;
    }
    if (READ_BLOCK != read_status) return read_status;
  }
  /* Read the next block.
    Important note: we probably speed things up by removing "data"
    and the memcpy to it by extending vcd_image_source_read_mode2
    to allow a mode to do what's below in addition to its
    "raw" and "block" mode. It also would probably improve the modularity
    a little bit as well.
  */
  {
    CdIo *p_img = vcdinfo_get_cd_image(p_vcdplayer->vcd);
    typedef struct {
      uint8_t subheader [CDIO_CD_SUBHEADER_SIZE];
      uint8_t data    [M2F2_SECTOR_SIZE];
      uint8_t spare     [4];
    } vcdsector_t;
    vcdsector_t vcd_sector;
    do {
      if (cdio_read_mode2_sector(p_img, &vcd_sector,
                                 p_vcdplayer->i_lsn, true)!=0) {
        dbg_print(INPUT_DBG_LSN, "read error\n");
        p_vcdplayer->i_lsn++;
        return READ_ERROR;
      }
      p_vcdplayer->i_lsn++;
      if ( p_vcdplayer->i_lsn >= p_vcdplayer->end_lsn ) {
        /* We've run off of the end of this entry. Do we continue or stop? */
        dbg_print( (INPUT_DBG_LSN|INPUT_DBG_PBC),
                   "end reached in reading, cur: %u, end: %u\n",
                   p_vcdplayer->i_lsn, p_vcdplayer->end_lsn);
        break;
      }
      /* Check header ID for a padding sector and simply discard
         these.  It is alleged that VCD's put these in to keep the
         bitrate constant.
      */
    } while((vcd_sector.subheader[2]&~0x01)==0x60);
    if ( p_vcdplayer->i_lsn >= p_vcdplayer->end_lsn )
      /* We've run off of the end of this entry. Do we continue or stop? */
      goto handle_item_continuation;
    memcpy (p_buf, vcd_sector.data, M2F2_SECTOR_SIZE);
    return READ_BLOCK;
  }
}
