} window_t;
static int vout_window_start(void *func, va_list ap)
{
    int (*activate)(vout_window_t *, const vout_window_cfg_t *) = func;
    vout_window_t *wnd = va_arg(ap, vout_window_t *);
    const vout_window_cfg_t *cfg = va_arg(ap, const vout_window_cfg_t *);
    return activate(wnd, cfg);
}
