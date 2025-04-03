}
static void OSDTextDestroy(subpicture_t *subpic)
{
    subpicture_updater_sys_t *sys = subpic->updater.p_sys;
    free(sys->text);
    free(sys);
}
