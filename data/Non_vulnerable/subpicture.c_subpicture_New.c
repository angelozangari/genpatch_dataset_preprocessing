};
subpicture_t *subpicture_New( const subpicture_updater_t *p_upd )
{
    subpicture_t *p_subpic = calloc( 1, sizeof(*p_subpic) );
    if( !p_subpic )
        return NULL;
    p_subpic->i_order    = 0;
    p_subpic->b_absolute = true;
    p_subpic->b_fade     = false;
    p_subpic->b_subtitle = false;
    p_subpic->i_alpha    = 0xFF;
    p_subpic->p_region   = NULL;
    if( p_upd )
    {
        subpicture_private_t *p_private = malloc( sizeof(*p_private) );
        if( !p_private )
        {
            free( p_subpic );
            return NULL;
        }
        video_format_Init( &p_private->src, 0 );
        video_format_Init( &p_private->dst, 0 );
        p_subpic->updater   = *p_upd;
        p_subpic->p_private = p_private;
    }
    else
    {
        p_subpic->p_private = NULL;
        p_subpic->updater.pf_validate = NULL;
        p_subpic->updater.pf_update   = NULL;
        p_subpic->updater.pf_destroy  = NULL;
        p_subpic->updater.p_sys       = NULL;
    }
    return p_subpic;
}
