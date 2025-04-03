}
void subpicture_region_Delete( subpicture_region_t *p_region )
{
    if( !p_region )
        return;
    if( p_region->p_private )
        subpicture_region_private_Delete( p_region->p_private );
    if( p_region->p_picture )
        picture_Release( p_region->p_picture );
    free( p_region->fmt.p_palette );
    free( p_region->psz_text );
    free( p_region->psz_html );
    if( p_region->p_style )
        text_style_Delete( p_region->p_style );
    free( p_region );
}
