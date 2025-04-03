};
static void callback (void *ptr)
{
    struct timer_data *data = ptr;
    vlc_mutex_lock (&data->lock);
    data->count += 1 + vlc_timer_getoverrun (data->timer);
    vlc_mutex_unlock (&data->lock);
}
