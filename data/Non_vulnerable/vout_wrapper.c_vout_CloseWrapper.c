 *****************************************************************************/
void vout_CloseWrapper(vout_thread_t *vout, vout_display_state_t *state)
{
    vout_thread_sys_t *sys = vout->p;
#ifdef _WIN32
    var_DelCallback(vout, "video-wallpaper", Forward, NULL);
#endif
    sys->decoder_pool = NULL; /* FIXME remove */
    vout_DeleteDisplay(sys->display.vd, state);
    free(sys->display.title);
}
