/* */
static vlm_media_sys_t *vlm_ControlMediaGetById( vlm_t *p_vlm, int64_t id )
{
    int i;
    for( i = 0; i < p_vlm->i_media; i++ )
    {
        if( p_vlm->media[i]->cfg.id == id )
            return p_vlm->media[i];
    }
    return NULL;
}
