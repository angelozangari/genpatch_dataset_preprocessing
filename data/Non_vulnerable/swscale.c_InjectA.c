}
static void InjectA( picture_t *p_dst, const picture_t *restrict p_src,
                     unsigned i_width, unsigned i_height, unsigned offset )
{
    plane_t *d = &p_dst->p[0];
    const plane_t *s = &p_src->p[0];
    for( unsigned y = 0; y < i_height; y++ )
        for( unsigned x = 0; x < i_width; x++ )
            d->p_pixels[y*d->i_pitch+4*x+offset] = s->p_pixels[y*s->i_pitch+x];
}
