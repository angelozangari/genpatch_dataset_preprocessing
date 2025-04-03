/* */
static void Close(vlc_object_t *object)
{
    vout_display_t *vd = (vout_display_t *)object;
    Clean(vd);
    CommonClean(vd);
    free(vd->sys);
}
