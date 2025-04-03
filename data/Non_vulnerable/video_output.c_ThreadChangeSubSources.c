}
static void ThreadChangeSubSources(vout_thread_t *vout, const char *filters)
{
    spu_ChangeSources(vout->p->spu, filters);
}
