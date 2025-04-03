}
void subpicture_region_private_Delete( subpicture_region_private_t *p_private )
{
    if( p_private->p_picture )
        picture_Release( p_private->p_picture );
    free( p_private->fmt.p_palette );
    free( p_private );
}
