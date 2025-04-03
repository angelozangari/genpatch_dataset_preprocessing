}
static void ThreadDisplayOsdTitle(vout_thread_t *vout, const char *string)
{
    if (!vout->p->title.show)
        return;
    vout_OSDText(vout, SPU_DEFAULT_CHANNEL,
                 vout->p->title.position, INT64_C(1000) * vout->p->title.timeout,
                 string);
}
