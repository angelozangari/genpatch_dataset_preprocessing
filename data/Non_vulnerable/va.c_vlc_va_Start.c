#include "va.h"
static int vlc_va_Start(void *func, va_list ap)
{
    vlc_va_t *va = va_arg(ap, vlc_va_t *);
    AVCodecContext *ctx = va_arg(ap, AVCodecContext *);
    const es_format_t *fmt = va_arg(ap, const es_format_t *);
    int (*open)(vlc_va_t *, AVCodecContext *, const es_format_t *) = func;
    return open(va, ctx, fmt);
}
