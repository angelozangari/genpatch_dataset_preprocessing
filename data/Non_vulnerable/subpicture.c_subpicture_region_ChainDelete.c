}
void subpicture_region_ChainDelete( subpicture_region_t *p_head )
{
    while( p_head )
    {
        subpicture_region_t *p_next = p_head->p_next;
        subpicture_region_Delete( p_head );
        p_head = p_next;
    }
}
