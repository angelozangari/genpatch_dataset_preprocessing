 */
void vout_HoldPicture(vout_thread_t *vout, picture_t *picture)
{
    vlc_mutex_lock(&vout->p->picture_lock);
    picture_Hold(picture);
    vlc_mutex_unlock(&vout->p->picture_lock);
}
