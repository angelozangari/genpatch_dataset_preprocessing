}
int vout_window_Control(vout_window_t *window, int query, ...)
{
    va_list args;
    va_start(args, query);
    int ret = window->control(window, query, args);
    va_end(args);
    return ret;
}
