 *****************************************************************************/
static subpicture_t *DecodePacket( decoder_t *p_dec, block_t *p_data )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    subpicture_t  *p_spu;
    subpicture_region_t *p_region;
    video_format_t fmt;
    video_palette_t palette;
    int i;
    /* Allocate the subpicture internal data. */
    p_spu = decoder_NewSubpicture( p_dec, NULL );
    if( !p_spu ) return NULL;
    p_spu->i_start = p_data->i_pts;
    p_spu->i_stop  = p_data->i_pts + p_sys->i_duration;
    p_spu->b_ephemer = true;
    /* Create new subtitle region */
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = VLC_CODEC_YUVP;
    /**
       The video on which the subtitle sits, is scaled, probably
       4:3. However subtitle bitmaps assume an 1:1 aspect ratio.
       FIXME: We should get the video aspect ratio from somewhere.
       Two candidates are the video and the other possibility would be
       the access module.
    */
    fmt.i_sar_num = p_sys->i_height;
    fmt.i_sar_den = p_sys->i_width;
    fmt.i_width = fmt.i_visible_width = p_sys->i_width;
    fmt.i_height = fmt.i_visible_height = p_sys->i_height;
    fmt.i_x_offset = fmt.i_y_offset = 0;
    fmt.p_palette = &palette;
    fmt.p_palette->i_entries = 4;
    for( i = 0; i < fmt.p_palette->i_entries; i++ )
    {
        fmt.p_palette->palette[i][0] = p_sys->p_palette[i][0];
        fmt.p_palette->palette[i][1] = p_sys->p_palette[i][1];
        fmt.p_palette->palette[i][2] = p_sys->p_palette[i][2];
        fmt.p_palette->palette[i][3] = p_sys->p_palette[i][3];
    }
    p_region = subpicture_region_New( &fmt );
    if( !p_region )
    {
        msg_Err( p_dec, "cannot allocate SVCD subtitle region" );
        decoder_DeleteSubpicture( p_dec, p_spu );
        return NULL;
    }
    p_spu->p_region = p_region;
    p_region->i_x = p_sys->i_x_start;
    p_region->i_y = p_sys->i_y_start;
    SVCDSubRenderImage( p_dec, p_data, p_region );
    return p_spu;
}
