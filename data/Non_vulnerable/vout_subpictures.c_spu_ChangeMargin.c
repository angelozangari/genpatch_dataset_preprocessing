}
void spu_ChangeMargin(spu_t *spu, int margin)
{
    spu_private_t *sys = spu->p;
    vlc_mutex_lock(&sys->lock);
    sys->margin = margin;
    vlc_mutex_unlock(&sys->lock);
}
