}
static int AboveCallback( vlc_object_t *obj, char const *name,
                          vlc_value_t prev, vlc_value_t cur, void *data )
{
    vout_ControlChangeWindowState( (vout_thread_t *)obj,
        cur.b_bool ? VOUT_WINDOW_STATE_ABOVE : VOUT_WINDOW_STATE_NORMAL );
    (void) name; (void) prev; (void) data;
    return VLC_SUCCESS;
}
