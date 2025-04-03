}
static void vout_window_stop(void *func, va_list ap)
{
    int (*deactivate)(vout_window_t *) = func;
    vout_window_t *wnd = va_arg(ap, vout_window_t *);
    deactivate(wnd);
}
