}
void vout_OSDText(vout_thread_t *vout, int channel,
                   int position, mtime_t duration, const char *text)
{
    assert( (position & ~SUBPICTURE_ALIGN_MASK) == 0);
    if (!var_InheritBool(vout, "osd") || duration <= 0)
        return;
    subpicture_updater_sys_t *sys = malloc(sizeof(*sys));
    if (!sys)
        return;
    sys->position = position;
    sys->text     = strdup(text);
    subpicture_updater_t updater = {
        .pf_validate = OSDTextValidate,
        .pf_update   = OSDTextUpdate,
        .pf_destroy  = OSDTextDestroy,
        .p_sys       = sys,
    };
    subpicture_t *subpic = subpicture_New(&updater);
    if (!subpic) {
        free(sys->text);
        free(sys);
        return;
    }
    subpic->i_channel  = channel;
    subpic->i_start    = mdate();
    subpic->i_stop     = subpic->i_start + duration;
    subpic->b_ephemer  = true;
    subpic->b_absolute = false;
    subpic->b_fade     = true;
    vout_PutSubpicture(vout, subpic);
}
