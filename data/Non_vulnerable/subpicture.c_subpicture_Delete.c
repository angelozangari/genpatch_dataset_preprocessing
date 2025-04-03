}
void subpicture_Delete( subpicture_t *p_subpic )
{
    subpicture_region_ChainDelete( p_subpic->p_region );
    p_subpic->p_region = NULL;
    if( p_subpic->updater.pf_destroy )
        p_subpic->updater.pf_destroy( p_subpic );
    free( p_subpic->p_private );
    free( p_subpic );
}
