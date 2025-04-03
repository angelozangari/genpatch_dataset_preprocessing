}
static void OSDWidget(vout_thread_t *vout, int channel, int type, int position)
{
    if (!var_InheritBool(vout, "osd"))
        return;
    if (type == OSD_HOR_SLIDER || type == OSD_VERT_SLIDER)
        position = VLC_CLIP(position, 0, 100);
    subpicture_updater_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return;
    sys->type     = type;
    sys->position = position;
    subpicture_updater_t updater = {
        .pf_validate = OSDWidgetValidate,
        .pf_update   = OSDWidgetUpdate,
        .pf_destroy  = OSDWidgetDestroy,
        .p_sys       = sys,
    };
    subpicture_t *subpic = subpicture_New(&updater);
    if (!subpic) {
        free(sys);
        return;
    }
    subpic->i_channel  = channel;
    subpic->i_start    = mdate();
    subpic->i_stop     = subpic->i_start + 1200000;
    subpic->b_ephemer  = true;
    subpic->b_absolute = true;
    subpic->b_fade     = true;
    vout_PutSubpicture(vout, subpic);
}
