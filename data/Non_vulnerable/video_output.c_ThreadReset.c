}
static void ThreadReset(vout_thread_t *vout)
{
    ThreadFlush(vout, true, INT64_MAX);
    if (vout->p->decoder_pool)
        picture_pool_NonEmpty(vout->p->decoder_pool, true);
    vout->p->pause.is_on = false;
    vout->p->pause.date  = mdate();
}
