#ifdef _WIN32
static int Forward(vlc_object_t *object, char const *var,
                   vlc_value_t oldval, vlc_value_t newval, void *data)
{
    vout_thread_t *vout = (vout_thread_t*)object;
    VLC_UNUSED(oldval);
    VLC_UNUSED(data);
    return var_Set(vout->p->display.vd, var, newval);
}
