}
static void ThreadDisplaySubpicture(vout_thread_t *vout,
                                    subpicture_t *subpicture)
{
    spu_PutSubpicture(vout->p->spu, subpicture);
}
