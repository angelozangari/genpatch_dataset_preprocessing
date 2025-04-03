}
int vout_RegisterSubpictureChannel( vout_thread_t *vout )
{
    int channel = SPU_DEFAULT_CHANNEL;
    vlc_mutex_lock(&vout->p->spu_lock);
    if (vout->p->spu)
        channel = spu_RegisterChannel(vout->p->spu);
    vlc_mutex_unlock(&vout->p->spu_lock);
    return channel;
}
