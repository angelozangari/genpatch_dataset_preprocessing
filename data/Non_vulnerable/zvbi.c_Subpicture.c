}
static subpicture_t *Subpicture( decoder_t *p_dec, video_format_t *p_fmt,
                                 bool b_text,
                                 int i_columns, int i_rows, int i_align,
                                 mtime_t i_pts )
{
    video_format_t fmt;
    subpicture_t *p_spu=NULL;
    /* If there is a page or sub to render, then we do that here */
    /* Create the subpicture unit */
    if( b_text )
        p_spu = decoder_NewSubpictureText( p_dec );
    else
        p_spu = decoder_NewSubpicture( p_dec, NULL );
    if( !p_spu )
    {
        msg_Warn( p_dec, "can't get spu buffer" );
        return NULL;
    }
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = b_text ? VLC_CODEC_TEXT : VLC_CODEC_RGBA;
    fmt.i_sar_num = 0;
    fmt.i_sar_den = 1;
    if( b_text )
    {
        fmt.i_bits_per_pixel = 0;
    }
    else
    {
        fmt.i_width = fmt.i_visible_width = i_columns * 12;
        fmt.i_height = fmt.i_visible_height = i_rows * 10;
        fmt.i_bits_per_pixel = 32;
        fmt.i_sar_num = fmt.i_sar_den = 0; /* let the vout set the correct AR */
    }
    fmt.i_x_offset = fmt.i_y_offset = 0;
    p_spu->p_region = subpicture_region_New( &fmt );
    if( p_spu->p_region == NULL )
    {
        msg_Err( p_dec, "cannot allocate SPU region" );
        decoder_DeleteSubpicture( p_dec, p_spu );
        return NULL;
    }
    p_spu->p_region->i_x = 0;
    p_spu->p_region->i_y = 0;
    p_spu->i_start = i_pts;
    p_spu->i_stop = b_text ? i_pts + (10*CLOCK_FREQ): 0;
    p_spu->b_ephemer = true;
    p_spu->b_absolute = b_text ? false : true;
    if( !b_text )
        p_spu->p_region->i_align = i_align;
    p_spu->i_original_picture_width = fmt.i_width;
    p_spu->i_original_picture_height = fmt.i_height;
    /* */
    *p_fmt = fmt;
    return p_spu;
}
