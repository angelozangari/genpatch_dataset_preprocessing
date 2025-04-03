}
static void ThreadChangeSubMargin(vout_thread_t *vout, int margin)
{
    spu_ChangeMargin(vout->p->spu, margin);
}
