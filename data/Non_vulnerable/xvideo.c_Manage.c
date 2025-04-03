}
static void Manage (vout_display_t *vd)
{
    vout_display_sys_t *p_sys = vd->sys;
    XCB_Manage (vd, p_sys->conn, &p_sys->visible);
}
