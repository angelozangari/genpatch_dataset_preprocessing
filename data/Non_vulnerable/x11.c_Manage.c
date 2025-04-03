}
static void Manage (vout_display_t *vd)
{
    vout_display_sys_t *sys = vd->sys;
    XCB_Manage (vd, sys->conn, &sys->visible);
}
