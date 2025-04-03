 *****************************************************************************/
static rfbBool mallocFrameBufferHandler( rfbClient* p_client )
{
    vlc_fourcc_t i_chroma;
    demux_t *p_demux = (demux_t *) rfbClientGetClientData( p_client, DemuxThread );
    demux_sys_t *p_sys = p_demux->p_sys;
    if ( p_sys->es ) /* Source has changed resolution */
    {
        es_out_Del( p_demux->out, p_sys->es );
        p_sys->es = NULL;
    }
    int i_width = p_client->width;
    int i_height = p_client->height;
    int i_depth = p_client->format.bitsPerPixel;
    switch( i_depth )
    {
        case 8:
            i_chroma = VLC_CODEC_RGB8;
            break;
        default:
        case 16:
            i_chroma = VLC_CODEC_RGB16;
            break;
        case 24:
            i_chroma = VLC_CODEC_RGB24;
            break;
        case 32:
            i_chroma = VLC_CODEC_RGB32;
            break;
    }
    if ( i_chroma != VLC_CODEC_RGB8 ) /* Palette based, no mask */
    {
        video_format_t videofmt;
        memset( &videofmt, 0, sizeof(video_format_t) );
        videofmt.i_chroma = i_chroma;
        video_format_FixRgb( &videofmt );
        p_client->format.redShift = videofmt.i_lrshift;
        p_client->format.greenShift = videofmt.i_lgshift;
        p_client->format.blueShift = videofmt.i_lbshift;
        p_client->format.redMax = videofmt.i_rmask >> videofmt.i_lrshift;
        p_client->format.greenMax = videofmt.i_gmask >> videofmt.i_lgshift;
        p_client->format.blueMax = videofmt.i_bmask >> videofmt.i_lbshift;
    }
    /* Set up framebuffer */
    p_sys->i_framebuffersize = i_width * i_height * i_depth / 8;
    /* Reuse unsent block */
    if ( p_sys->p_block )
        p_sys->p_block = block_Realloc( p_sys->p_block, 0, p_sys->i_framebuffersize );
    else
        p_sys->p_block = block_Alloc( p_sys->i_framebuffersize );
    if ( p_sys->p_block )
        p_sys->p_block->i_buffer = p_sys->i_framebuffersize;
    else
        return FALSE;
    /* Push our VNC config */
    SetFormatAndEncodings( p_client );
    /* Now init and fill es format */
    es_format_t fmt;
    es_format_Init( &fmt, VIDEO_ES, i_chroma );
    /* Fill input format */
    fmt.video.i_chroma = i_chroma;
    fmt.video.i_visible_width =
            fmt.video.i_width = i_width;
    fmt.video.i_visible_height =
            fmt.video.i_height = i_height;
    fmt.video.i_frame_rate_base = 1000;
    fmt.video.i_frame_rate = 1000 * p_sys->f_fps;
    fmt.video.i_bits_per_pixel = i_depth;
    fmt.video.i_rmask = p_client->format.redMax << p_client->format.redShift;
    fmt.video.i_gmask = p_client->format.greenMax << p_client->format.greenShift;
    fmt.video.i_bmask = p_client->format.blueMax << p_client->format.blueShift;
    fmt.video.i_sar_num = fmt.video.i_sar_den = 1;
    /* declare the new es */
    p_sys->es = es_out_Add( p_demux->out, &fmt );
    return TRUE;
}
