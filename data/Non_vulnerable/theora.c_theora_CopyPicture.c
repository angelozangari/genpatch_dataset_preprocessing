 *****************************************************************************/
static void theora_CopyPicture( picture_t *p_pic,
                                th_ycbcr_buffer ycbcr )
{
    int i_plane, i_planes, i_line, i_dst_stride, i_src_stride;
    uint8_t *p_dst, *p_src;
    /* th_img_plane
       int  width   The width of this plane.
       int  height  The height of this plane.
       int  stride  The offset in bytes between successive rows.
       unsigned char *data  A pointer to the beginning of the first row.
       Detailed Description
       A buffer for a single color plane in an uncompressed image.
       This contains the image data in a left-to-right, top-down
       format. Each row of pixels is stored contiguously in memory,
       but successive rows need not be. Use stride to compute the
       offset of the next row. The encoder accepts both positive
       stride values (top-down in memory) and negative (bottom-up in
       memory). The decoder currently always generates images with
       positive strides.
       typedef th_img_plane th_ycbcr_buffer[3]
    */
    i_planes = p_pic->i_planes < 3 ? p_pic->i_planes : 3;
    for( i_plane = 0; i_plane < i_planes; i_plane++ )
    {
        p_dst = p_pic->p[i_plane].p_pixels;
        p_src = ycbcr[i_plane].data;
        i_dst_stride  = p_pic->p[i_plane].i_pitch;
        i_src_stride  = ycbcr[i_plane].stride;
        for( i_line = 0;
             i_line < __MIN(p_pic->p[i_plane].i_lines, ycbcr[i_plane].height);
             i_line++ )
        {
            memcpy( p_dst, p_src, ycbcr[i_plane].width );
            p_src += i_src_stride;
            p_dst += i_dst_stride;
        }
    }
}
