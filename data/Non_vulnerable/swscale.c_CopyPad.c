}
static void CopyPad( picture_t *p_dst, const picture_t *p_src )
{
    picture_Copy( p_dst, p_src );
    for( int n = 0; n < p_dst->i_planes; n++ )
    {
        const plane_t *s = &p_src->p[n];
        plane_t *d = &p_dst->p[n];
        for( int y = 0; y < s->i_lines && y < d->i_lines; y++ )
        {
            for( int x = s->i_visible_pitch; x < d->i_visible_pitch; x += s->i_pixel_pitch )
                memcpy( &d->p_pixels[y*d->i_pitch + x], &d->p_pixels[y*d->i_pitch + s->i_visible_pitch - s->i_pixel_pitch], s->i_pixel_pitch );
        }
    }
}
