 */
void vout_ReleasePicture(vout_thread_t *vout, picture_t *picture)
{
    vlc_mutex_lock(&vout->p->picture_lock);
    picture_Release(picture);
    vlc_mutex_unlock(&vout->p->picture_lock);
    vout_control_Wake(&vout->p->control);
}
