 */
static void Close (vlc_object_t *obj)
{
    vout_display_t *vd = (vout_display_t *)obj;
    vout_display_sys_t *p_sys = vd->sys;
    if (p_sys->pool)
        picture_pool_Delete (p_sys->pool);
    /* show the default cursor */
    xcb_change_window_attributes (p_sys->conn, p_sys->embed->handle.xid, XCB_CW_CURSOR,
                                  &(uint32_t) { XCB_CURSOR_NONE });
    xcb_flush (p_sys->conn);
    free (p_sys->att);
    xcb_disconnect (p_sys->conn);
    vout_display_DeleteWindow (vd, p_sys->embed);
    free (p_sys);
}
