}
static void FillA( plane_t *d, unsigned i_offset )
{
    for( int y = 0; y < d->i_visible_lines; y++ )
        for( int x = 0; x < d->i_visible_pitch; x += d->i_pixel_pitch )
            d->p_pixels[y*d->i_pitch+x+i_offset] = 0xff;
}
