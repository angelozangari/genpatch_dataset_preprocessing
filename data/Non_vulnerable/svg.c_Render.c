 *****************************************************************************/
static int Render( filter_t *p_filter, subpicture_region_t *p_region,
                   svg_rendition_t *p_svg, int i_width, int i_height )
{
    video_format_t fmt;
    uint8_t *p_y, *p_u, *p_v, *p_a;
    int x, y, i_pitch, i_u_pitch;
    guchar *pixels_in = NULL;
    int rowstride_in;
    int channels_in;
    int alpha;
    picture_t *p_pic;
    if ( p_filter->p_sys->i_width != i_width ||
         p_filter->p_sys->i_height != i_height )
    {
        svg_set_size( p_filter, i_width, i_height );
        p_svg->p_rendition = NULL;
    }
    if( p_svg->p_rendition == NULL ) {
        svg_RenderPicture( p_filter, p_svg );
        if( ! p_svg->p_rendition )
        {
            msg_Err( p_filter, "Cannot render SVG" );
            return VLC_EGENERIC;
        }
    }
    i_width = gdk_pixbuf_get_width( p_svg->p_rendition );
    i_height = gdk_pixbuf_get_height( p_svg->p_rendition );
    /* Create a new subpicture region */
    memset( &fmt, 0, sizeof( video_format_t ) );
    fmt.i_chroma = VLC_CODEC_YUVA;
    fmt.i_width = fmt.i_visible_width = i_width;
    fmt.i_height = fmt.i_visible_height = i_height;
    fmt.i_x_offset = fmt.i_y_offset = 0;
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    p_region->p_picture = picture_NewFromFormat( &fmt );
    if( !p_region->p_picture )
        return VLC_EGENERIC;
    p_region->fmt = fmt;
    p_region->i_x = p_region->i_y = 0;
    p_y = p_region->p_picture->Y_PIXELS;
    p_u = p_region->p_picture->U_PIXELS;
    p_v = p_region->p_picture->V_PIXELS;
    p_a = p_region->p_picture->A_PIXELS;
    i_pitch = p_region->p_picture->Y_PITCH;
    i_u_pitch = p_region->p_picture->U_PITCH;
    /* Initialize the region pixels (only the alpha will be changed later) */
    memset( p_y, 0x00, i_pitch * p_region->fmt.i_height );
    memset( p_u, 0x80, i_u_pitch * p_region->fmt.i_height );
    memset( p_v, 0x80, i_u_pitch * p_region->fmt.i_height );
    p_pic = p_region->p_picture;
    /* Copy the data */
    /* This rendering code is in no way optimized. If someone has some time to
       make it work faster or better, please do.
    */
    /*
      p_pixbuf->get_rowstride() is the number of bytes in a line.
      p_pixbuf->get_height() is the number of lines.
      The number of bytes of p_pixbuf->p_pixels is get_rowstride * get_height
      if( has_alpha() ) {
      alpha = pixels [ n_channels * ( y*rowstride + x ) + 3 ];
      }
      red   = pixels [ n_channels * ( y*rowstride ) + x ) ];
      green = pixels [ n_channels * ( y*rowstride ) + x ) + 1 ];
      blue  = pixels [ n_channels * ( y*rowstride ) + x ) + 2 ];
    */
    pixels_in = gdk_pixbuf_get_pixels( p_svg->p_rendition );
    rowstride_in = gdk_pixbuf_get_rowstride( p_svg->p_rendition );
    channels_in = gdk_pixbuf_get_n_channels( p_svg->p_rendition );
    alpha = gdk_pixbuf_get_has_alpha( p_svg->p_rendition );
    /*
      This crashes the plugin (if !alpha). As there is always an alpha value,
      it does not matter for the moment :
    if( !alpha )
      memset( p_a, 0xFF, i_pitch * p_region->fmt.i_height );
    */
#define INDEX_IN( x, y ) ( y * rowstride_in + x * channels_in )
#define INDEX_OUT( x, y ) ( y * i_pitch + x * p_pic->p[Y_PLANE].i_pixel_pitch )
    for( y = 0; y < i_height; y++ )
    {
        for( x = 0; x < i_width; x++ )
        {
            guchar *p_in;
            int i_out;
            p_in = &pixels_in[INDEX_IN( x, y )];
#define R( pixel ) *pixel
#define G( pixel ) *( pixel+1 )
#define B( pixel ) *( pixel+2 )
#define ALPHA( pixel ) *( pixel+3 )
            /* From http://www.geocrawler.com/archives/3/8263/2001/6/0/6020594/ :
               Y = 0.29900 * R + 0.58700 * G + 0.11400 * B
               U = -0.1687 * r  - 0.3313 * g + 0.5 * b + 128
               V = 0.5   * r - 0.4187 * g - 0.0813 * b + 128
            */
            if ( alpha ) {
                i_out = INDEX_OUT( x, y );
                p_pic->Y_PIXELS[i_out] = .299 * R( p_in ) + .587 * G( p_in ) + .114 * B( p_in );
                p_pic->U_PIXELS[i_out] = -.1687 * R( p_in ) - .3313 * G( p_in ) + .5 * B( p_in ) + 128;
                p_pic->V_PIXELS[i_out] = .5 * R( p_in ) - .4187 * G( p_in ) - .0813 * B( p_in ) + 128;
                p_pic->A_PIXELS[i_out] = ALPHA( p_in );
            }
        }
    }
    return VLC_SUCCESS;
}
