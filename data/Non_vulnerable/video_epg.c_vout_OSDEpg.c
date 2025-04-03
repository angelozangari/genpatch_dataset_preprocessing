 */
int vout_OSDEpg(vout_thread_t *vout, input_item_t *input)
{
    char *now_playing = input_item_GetNowPlayingFb(input);
    vlc_epg_t *epg = NULL;
    vlc_mutex_lock(&input->lock);
    /* Look for the current program EPG event */
    for (int i = 0; i < input->i_epg; i++) {
        vlc_epg_t *tmp = input->pp_epg[i];
        if (tmp->p_current &&
            tmp->p_current->psz_name && now_playing != NULL &&
            !strcmp(tmp->p_current->psz_name, now_playing)) {
            epg = vlc_epg_New(tmp->psz_name);
            vlc_epg_Merge(epg, tmp);
            break;
        }
    }
    vlc_mutex_unlock(&input->lock);
    free(now_playing);
    /* If no EPG event has been found. */
    if (epg == NULL)
        return VLC_EGENERIC;
    subpicture_updater_sys_t *sys = malloc(sizeof(*sys));
    if (!sys) {
        vlc_epg_Delete(epg);
        return VLC_EGENERIC;
    }
    sys->epg = epg;
    subpicture_updater_t updater = {
        .pf_validate = OSDEpgValidate,
        .pf_update   = OSDEpgUpdate,
        .pf_destroy  = OSDEpgDestroy,
        .p_sys       = sys
    };
    const mtime_t now = mdate();
    subpicture_t *subpic = subpicture_New(&updater);
    if (!subpic) {
        vlc_epg_Delete(sys->epg);
        free(sys);
        return VLC_EGENERIC;
    }
    subpic->i_channel  = SPU_DEFAULT_CHANNEL;
    subpic->i_start    = now;
    subpic->i_stop     = now + 3000 * INT64_C(1000);
    subpic->b_ephemer  = true;
    subpic->b_absolute = true;
    subpic->b_fade     = true;
    vout_PutSubpicture(vout, subpic);
    return VLC_SUCCESS;
}
