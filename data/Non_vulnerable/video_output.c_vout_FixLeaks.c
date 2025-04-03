}
void vout_FixLeaks( vout_thread_t *vout )
{
    vlc_mutex_lock(&vout->p->picture_lock);
    picture_t *picture = picture_fifo_Peek(vout->p->decoder_fifo);
    if (!picture) {
        picture = picture_pool_Get(vout->p->decoder_pool);
    }
    if (picture) {
        picture_Release(picture);
        /* Not all pictures has been displayed yet or some are
         * free */
        vlc_mutex_unlock(&vout->p->picture_lock);
        return;
    }
    /* There is no reason that no pictures are available, force one
     * from the pool, becarefull with it though */
    msg_Err(vout, "pictures leaked, trying to workaround");
    /* */
    picture_pool_NonEmpty(vout->p->decoder_pool, false);
    vlc_mutex_unlock(&vout->p->picture_lock);
}
