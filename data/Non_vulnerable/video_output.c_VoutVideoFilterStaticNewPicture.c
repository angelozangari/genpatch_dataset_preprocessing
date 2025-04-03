}
static picture_t *VoutVideoFilterStaticNewPicture(filter_t *filter)
{
    vout_thread_t *vout = (vout_thread_t*)filter->p_owner;
    vlc_assert_locked(&vout->p->filter.lock);
    if (filter_chain_GetLength(vout->p->filter.chain_interactive) == 0)
        return VoutVideoFilterInteractiveNewPicture(filter);
    return picture_NewFromFormat(&filter->fmt_out.video);
}
