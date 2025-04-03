}
static void OSDWidgetDestroy(subpicture_t *subpic)
{
    free(subpic->updater.p_sys);
}
