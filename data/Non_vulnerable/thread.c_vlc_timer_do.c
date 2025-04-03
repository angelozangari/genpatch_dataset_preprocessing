};
static void CALLBACK vlc_timer_do (void *val, BOOLEAN timeout)
{
    struct vlc_timer *timer = val;
    assert (timeout);
    timer->func (timer->data);
}
