}
static int vlm_ControlMediaGets( vlm_t *p_vlm, vlm_media_t ***ppp_dsc, int *pi_dsc )
{
    vlm_media_t **pp_dsc;
    int                     i_dsc;
    int i;
    TAB_INIT( i_dsc, pp_dsc );
    for( i = 0; i < p_vlm->i_media; i++ )
    {
        vlm_media_t *p_dsc = vlm_media_Duplicate( &p_vlm->media[i]->cfg );
        TAB_APPEND( i_dsc, pp_dsc, p_dsc );
    }
    *ppp_dsc = pp_dsc;
    *pi_dsc = i_dsc;
    return VLC_SUCCESS;
}
