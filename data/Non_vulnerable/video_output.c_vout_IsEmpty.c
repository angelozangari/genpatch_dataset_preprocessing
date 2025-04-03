}
bool vout_IsEmpty(vout_thread_t *vout)
{
    vlc_mutex_lock(&vout->p->picture_lock);
    picture_t *picture = picture_fifo_Peek(vout->p->decoder_fifo);
    if (picture)
        picture_Release(picture);
    vlc_mutex_unlock(&vout->p->picture_lock);
    return !picture;
}
