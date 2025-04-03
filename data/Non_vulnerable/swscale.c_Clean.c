}
static void Clean( filter_t *p_filter )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    if( p_sys->p_src_e )
        picture_Release( p_sys->p_src_e );
    if( p_sys->p_dst_e )
        picture_Release( p_sys->p_dst_e );
    if( p_sys->p_src_a )
        picture_Release( p_sys->p_src_a );
    if( p_sys->p_dst_a )
        picture_Release( p_sys->p_dst_a );
    if( p_sys->ctxA )
        sws_freeContext( p_sys->ctxA );
    if( p_sys->ctx )
        sws_freeContext( p_sys->ctx );
    /* We have to set it to null has we call be called again :( */
    p_sys->ctx = NULL;
    p_sys->ctxA = NULL;
    p_sys->p_src_a = NULL;
    p_sys->p_dst_a = NULL;
    p_sys->p_src_e = NULL;
    p_sys->p_dst_e = NULL;
}
