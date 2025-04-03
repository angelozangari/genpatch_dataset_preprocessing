}
void vout_Close(vout_thread_t *vout)
{
    assert(vout);
    if (vout->p->input)
        spu_Attach(vout->p->spu, vout->p->input, false);
    vout_snapshot_End(&vout->p->snapshot);
    vout_control_PushVoid(&vout->p->control, VOUT_CONTROL_CLEAN);
    vlc_join(vout->p->thread, NULL);
    vlc_mutex_lock(&vout->p->spu_lock);
    spu_Destroy(vout->p->spu);
    vout->p->spu = NULL;
    vlc_mutex_unlock(&vout->p->spu_lock);
}
