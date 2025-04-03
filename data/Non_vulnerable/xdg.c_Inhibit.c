}
static void Inhibit (vlc_inhibit_t *ih, unsigned mask)
{
    vlc_inhibit_sys_t *sys = ih->p_sys;
    bool suspend = (mask & VLC_INHIBIT_DISPLAY) != 0;
    mtime_t delay = suspend ? 30 * CLOCK_FREQ : INT64_C(0);
    vlc_timer_schedule (sys->timer, false, delay, delay);
}
