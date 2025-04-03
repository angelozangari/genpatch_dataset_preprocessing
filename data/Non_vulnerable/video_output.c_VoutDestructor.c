/* */
static void VoutDestructor(vlc_object_t *object)
{
    vout_thread_t *vout = (vout_thread_t *)object;
    /* Make sure the vout was stopped first */
    //assert(!vout->p_module);
    free(vout->p->splitter_name);
    /* Destroy the locks */
    vlc_mutex_destroy(&vout->p->spu_lock);
    vlc_mutex_destroy(&vout->p->picture_lock);
    vlc_mutex_destroy(&vout->p->filter.lock);
    vout_control_Clean(&vout->p->control);
    /* */
    vout_statistic_Clean(&vout->p->statistic);
    /* */
    vout_snapshot_Clean(&vout->p->snapshot);
    video_format_Clean(&vout->p->original);
}
