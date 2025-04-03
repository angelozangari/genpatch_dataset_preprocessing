#pragma mark - actual data handling
static void vda_Copy420YpCbCr8Planar( picture_t *p_pic,
                                      CVPixelBufferRef buffer,
                                      unsigned i_width,
                                      unsigned i_height,
                                      copy_cache_t *cache )
{
    uint8_t *pp_plane[3];
    size_t  pi_pitch[3];
    if (!buffer)
        return;
    CVPixelBufferLockBaseAddress( buffer, 0 );
    for( int i = 0; i < 3; i++ )
    {
        pp_plane[i] = CVPixelBufferGetBaseAddressOfPlane( buffer, i );
        pi_pitch[i] = CVPixelBufferGetBytesPerRowOfPlane( buffer, i );
    }
    CopyFromYv12( p_pic, pp_plane, pi_pitch,
                  i_width, i_height, cache );
    CVPixelBufferUnlockBaseAddress( buffer, 0 );
}
