 *****************************************************************************/
static int Render( filter_t *p_filter, subpicture_region_t *p_region,
                   uint8_t *p_bitmap, int i_width, int i_height )
{
    uint8_t *p_dst;
    video_format_t fmt;
    int i, i_pitch;
    bool b_outline = true;
    /* Create a new subpicture region */
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = VLC_CODEC_YUVP;
    fmt.i_width = fmt.i_visible_width = i_width + (b_outline ? 4 : 0);
    fmt.i_height = fmt.i_visible_height = i_height + (b_outline ? 4 : 0);
    fmt.i_x_offset = fmt.i_y_offset = 0;
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    /* Build palette */
    fmt.p_palette = calloc( 1, sizeof(*fmt.p_palette) );
    if( !fmt.p_palette )
        return VLC_EGENERIC;
    fmt.p_palette->i_entries = 16;
    for( i = 0; i < fmt.p_palette->i_entries; i++ )
    {
        fmt.p_palette->palette[i][0] = pi_gamma[i];
        fmt.p_palette->palette[i][1] = 128;
        fmt.p_palette->palette[i][2] = 128;
        fmt.p_palette->palette[i][3] = pi_gamma[i];
    }
    p_region->p_picture = picture_NewFromFormat( &fmt );
    if( !p_region->p_picture )
    {
        free( fmt.p_palette );
        return VLC_EGENERIC;
    }
    p_region->fmt = fmt;
    p_dst = p_region->p_picture->Y_PIXELS;
    i_pitch = p_region->p_picture->Y_PITCH;
    if( b_outline )
    {
        memset( p_dst, 0, i_pitch * fmt.i_height );
        p_dst += p_region->p_picture->Y_PITCH * 2 + 2;
    }
    for( i = 0; i < i_height; i++ )
    {
        memcpy( p_dst, p_bitmap, i_width );
        p_bitmap += (i_width+3) & ~3;
        p_dst += i_pitch;
    }
    /* Outlining (find something better than nearest neighbour filtering ?) */
    if( b_outline )
    {
        uint8_t *p_top = p_dst; /* Use 1st line as a cache */
        uint8_t left, current;
        int x, y;
        p_dst = p_region->p_picture->Y_PIXELS;
        for( y = 1; y < (int)fmt.i_height - 1; y++ )
        {
            memcpy( p_top, p_dst, fmt.i_width );
            p_dst += i_pitch;
            left = 0;
            for( x = 1; x < (int)fmt.i_width - 1; x++ )
            {
                current = p_dst[x];
                p_dst[x] = ( 4 * (int)p_dst[x] + left + p_top[x] + p_dst[x+1] +
                             p_dst[x + i_pitch]) / 8;
                left = current;
            }
        }
        memset( p_top, 0, fmt.i_width );
    }
    return VLC_SUCCESS;
}
