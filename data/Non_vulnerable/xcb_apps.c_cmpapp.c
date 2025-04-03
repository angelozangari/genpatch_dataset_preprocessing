}
static int cmpapp (const void *a, const void *b)
{
    xcb_window_t wa = *(xcb_window_t *)a;
    xcb_window_t wb = *(xcb_window_t *)b;
    if (wa > wb)
        return 1;
    if (wa < wb)
        return -1;
    return 0;
} 
