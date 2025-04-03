}
static int AutoScaleCallback( vlc_object_t *obj, char const *name,
                              vlc_value_t prev, vlc_value_t cur, void *data )
{
    vout_thread_t *p_vout = (vout_thread_t *)obj;
    (void) name; (void) prev; (void) data;
    vout_ControlChangeDisplayFilled( p_vout, cur.b_bool );
    return VLC_SUCCESS;
}
