}
void vout_OSDMessage(vout_thread_t *vout, int channel, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char *string;
    if (vasprintf(&string, format, args) != -1) {
        vout_OSDText(vout, channel,
                     SUBPICTURE_ALIGN_TOP|SUBPICTURE_ALIGN_RIGHT, 1000000,
                     string);
        free(string);
    }
    va_end(args);
}
