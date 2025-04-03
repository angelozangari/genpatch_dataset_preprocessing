}
int spu_RegisterChannel(spu_t *spu)
{
    spu_private_t *sys = spu->p;
    vlc_mutex_lock(&sys->lock);
    int channel = sys->channel++;
    vlc_mutex_unlock(&sys->lock);
    return channel;
}
