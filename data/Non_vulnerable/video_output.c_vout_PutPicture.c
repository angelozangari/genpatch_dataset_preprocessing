 */
void vout_PutPicture(vout_thread_t *vout, picture_t *picture)
{
    vlc_mutex_lock(&vout->p->picture_lock);
    picture->p_next = NULL;
    picture_fifo_Push(vout->p->decoder_fifo, picture);
    vlc_mutex_unlock(&vout->p->picture_lock);
    vout_control_Wake(&vout->p->control);
}
