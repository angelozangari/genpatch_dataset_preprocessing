 */
static void Close (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *sys = vd->sys;
    ResetPictures (vd);
    /* show the default cursor */
    xcb_change_window_attributes (sys->conn, sys->embed->handle.xid, XCB_CW_CURSOR,
                                  &(uint32_t) { XCB_CURSOR_NONE });
    xcb_flush (sys->conn);
    /* colormap, window and context are garbage-collected by X */
    xcb_disconnect (sys->conn);
    vout_display_DeleteWindow (vd, sys->embed);
    free (sys);
}
