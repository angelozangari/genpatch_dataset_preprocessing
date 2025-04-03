} preparse_data_t;
static int InputEventPreparse( vlc_object_t *p_this, char const *psz_cmd,
                               vlc_value_t oldval, vlc_value_t newval, void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(psz_cmd); VLC_UNUSED(oldval);
    preparse_data_t *p_pre = p_data;
    if( newval.i_int == INPUT_EVENT_DEAD ||
        ( p_pre->b_mux && newval.i_int == INPUT_EVENT_ITEM_META ) )
        vlc_sem_post( p_pre->p_sem );
    return VLC_SUCCESS;
}
