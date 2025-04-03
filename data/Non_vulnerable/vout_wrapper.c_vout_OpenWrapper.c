 *****************************************************************************/
int vout_OpenWrapper(vout_thread_t *vout,
                     const char *splitter_name, const vout_display_state_t *state)
{
    vout_thread_sys_t *sys = vout->p;
    msg_Dbg(vout, "Opening vout display wrapper");
    /* */
    sys->display.title = var_InheritString(vout, "video-title");
    /* */
    const mtime_t double_click_timeout = 300000;
    const mtime_t hide_timeout = var_CreateGetInteger(vout, "mouse-hide-timeout") * 1000;
    if (splitter_name) {
        sys->display.vd = vout_NewSplitter(vout, &vout->p->original, state, "$vout", splitter_name,
                                           double_click_timeout, hide_timeout);
    } else {
        sys->display.vd = vout_NewDisplay(vout, &vout->p->original, state, "$vout",
                                          double_click_timeout, hide_timeout);
    }
    if (!sys->display.vd) {
        free(sys->display.title);
        return VLC_EGENERIC;
    }
    /* */
#ifdef _WIN32
    var_Create(vout, "video-wallpaper", VLC_VAR_BOOL|VLC_VAR_DOINHERIT);
    var_AddCallback(vout, "video-wallpaper", Forward, NULL);
#endif
    /* */
    sys->decoder_pool = NULL;
    return VLC_SUCCESS;
}
