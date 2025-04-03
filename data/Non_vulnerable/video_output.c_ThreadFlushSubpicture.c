}
static void ThreadFlushSubpicture(vout_thread_t *vout, int channel)
{
    spu_ClearChannel(vout->p->spu, channel);
}
