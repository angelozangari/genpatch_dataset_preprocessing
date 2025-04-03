}
static int vlm_ControlMediaClear( vlm_t *p_vlm )
{
    while( p_vlm->i_media > 0 )
        vlm_ControlMediaDel( p_vlm, p_vlm->media[0]->cfg.id );
    return VLC_SUCCESS;
}
