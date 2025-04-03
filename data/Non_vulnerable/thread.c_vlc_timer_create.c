}
int vlc_timer_create (vlc_timer_t *id, void (*func) (void *), void *data)
{
    struct vlc_timer *timer = malloc (sizeof (*timer));
    if (timer == NULL)
        return ENOMEM;
    timer->func = func;
    timer->data = data;
    timer->handle = INVALID_HANDLE_VALUE;
    *id = timer;
    return 0;
}
