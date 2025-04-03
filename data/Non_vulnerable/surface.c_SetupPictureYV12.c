#define ALIGN_16_PIXELS( x ) ( ( ( x ) + 15 ) / 16 * 16 )
static void SetupPictureYV12( SurfaceInfo* p_surfaceInfo, picture_t *p_picture )
{
    /* according to document of android.graphics.ImageFormat.YV12 */
    int i_stride = ALIGN_16_PIXELS( p_surfaceInfo->s );
    int i_c_stride = ALIGN_16_PIXELS( i_stride / 2 );
    p_picture->p->i_pitch = i_stride;
    /* Fill chroma planes for planar YUV */
    for( int n = 1; n < p_picture->i_planes; n++ )
    {
        const plane_t *o = &p_picture->p[n-1];
        plane_t *p = &p_picture->p[n];
        p->p_pixels = o->p_pixels + o->i_lines * o->i_pitch;
        p->i_pitch  = i_c_stride;
        p->i_lines  = p_picture->format.i_height / 2;
        /*
          Explicitly set the padding lines of the picture to black (127 for YUV)
          since they might be used by Android during rescaling.
        */
        int visible_lines = p_picture->format.i_visible_height / 2;
        if (visible_lines < p->i_lines)
            memset(&p->p_pixels[visible_lines * p->i_pitch], 127, (p->i_lines - visible_lines) * p->i_pitch);
    }
    if( vlc_fourcc_AreUVPlanesSwapped( p_picture->format.i_chroma,
                                       VLC_CODEC_YV12 ) ) {
        uint8_t *p_tmp = p_picture->p[1].p_pixels;
        p_picture->p[1].p_pixels = p_picture->p[2].p_pixels;
        p_picture->p[2].p_pixels = p_tmp;
    }
}
