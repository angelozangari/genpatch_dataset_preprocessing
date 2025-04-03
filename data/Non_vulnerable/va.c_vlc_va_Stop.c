}
static void vlc_va_Stop(void *func, va_list ap)
{
    vlc_va_t *va = va_arg(ap, vlc_va_t *);
    void (*close)(vlc_va_t *) = func;
    close(va);
}
