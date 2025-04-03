}
static void ThreadChangeSubFilters(vout_thread_t *vout, const char *filters)
{
    spu_ChangeFilters(vout->p->spu, filters);
}
