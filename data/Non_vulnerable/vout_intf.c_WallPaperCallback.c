}
static int WallPaperCallback( vlc_object_t *obj, char const *name,
                              vlc_value_t prev, vlc_value_t cur, void *data )
{
    vout_thread_t *vout = (vout_thread_t *)obj;
    if( cur.b_bool )
    {
        vout_ControlChangeWindowState( vout, VOUT_WINDOW_STATE_BELOW );
        vout_ControlChangeFullscreen( vout, true );
    }
    else
    {
        var_TriggerCallback( obj, "fullscreen" );
        var_TriggerCallback( obj, "video-on-top" );
    }
    (void) name; (void) prev; (void) data;
    return VLC_SUCCESS;
}
