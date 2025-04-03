}
int main (void)
{
    struct timer_data data;
    int val;
    vlc_mutex_init (&data.lock);
    data.count = 0;
    val = vlc_timer_create (&data.timer, callback, &data);
    assert (val == 0);
    /* Relative timer */
    vlc_timer_schedule (data.timer, false, 1, CLOCK_FREQ / 10);
    msleep (CLOCK_FREQ);
    vlc_mutex_lock (&data.lock);
    data.count += vlc_timer_getoverrun (data.timer);
    printf ("Count = %u\n", data.count);
    assert (data.count >= 10);
    data.count = 0;
    vlc_mutex_unlock (&data.lock);
    vlc_timer_schedule (data.timer, false, 0, 0);
    /* Absolute timer */
    mtime_t now = mdate ();
    vlc_timer_schedule (data.timer, true, now, CLOCK_FREQ / 10);
    msleep (CLOCK_FREQ);
    vlc_mutex_lock (&data.lock);
    data.count += vlc_timer_getoverrun (data.timer);
    printf ("Count = %u\n", data.count);
    assert (data.count >= 10);
    vlc_mutex_unlock (&data.lock);
    vlc_timer_destroy (data.timer);
    vlc_mutex_destroy (&data.lock);
    return 0;
}
