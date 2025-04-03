}
static void OSDEpgDestroy(subpicture_t *subpic)
{
    subpicture_updater_sys_t *sys = subpic->updater.p_sys;
    vlc_epg_Delete(sys->epg);
    free(sys);
}
