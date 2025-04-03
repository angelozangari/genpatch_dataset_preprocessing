static void Release( void *, uint8_t * );
static void vda_Copy422YpCbCr8( picture_t *p_pic,
                                CVPixelBufferRef buffer )
{
    int i_dst_stride, i_src_stride;
    uint8_t *p_dst, *p_src;
    CVPixelBufferLockBaseAddress( buffer, 0 );
    for( int i_plane = 0; i_plane < p_pic->i_planes; i_plane++ )
    {
        p_dst = p_pic->p[i_plane].p_pixels;
        p_src = CVPixelBufferGetBaseAddressOfPlane( buffer, i_plane );
        i_dst_stride  = p_pic->p[i_plane].i_pitch;
        i_src_stride  = CVPixelBufferGetBytesPerRowOfPlane( buffer, i_plane );
        for( int i_line = 0; i_line < p_pic->p[i_plane].i_visible_lines ; i_line++ )
        {
            memcpy( p_dst, p_src, i_src_stride );
            p_src += i_src_stride;
            p_dst += i_dst_stride;
        }
    }
    CVPixelBufferUnlockBaseAddress( buffer, 0 );
}
