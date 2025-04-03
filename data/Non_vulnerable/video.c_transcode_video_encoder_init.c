}
static void transcode_video_encoder_init( sout_stream_t *p_stream,
                                          sout_stream_id_sys_t *id )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    const es_format_t *p_fmt_out = &id->p_decoder->fmt_out;
    if( id->p_f_chain ) {
        p_fmt_out = filter_chain_GetFmtOut( id->p_f_chain );
    }
    if( id->p_uf_chain ) {
        p_fmt_out = filter_chain_GetFmtOut( id->p_uf_chain );
    }
    /* Calculate scaling
     * width/height of source */
    int i_src_visible_width = p_fmt_out->video.i_visible_width;
    int i_src_visible_height = p_fmt_out->video.i_visible_height;
    if (i_src_visible_width == 0)
        i_src_visible_width = p_fmt_out->video.i_width;
    if (i_src_visible_height == 0)
        i_src_visible_height = p_fmt_out->video.i_height;
    /* with/height scaling */
    float f_scale_width = 1;
    float f_scale_height = 1;
    /* aspect ratio */
    float f_aspect = (double)p_fmt_out->video.i_sar_num *
                     p_fmt_out->video.i_width /
                     p_fmt_out->video.i_sar_den /
                     p_fmt_out->video.i_height;
    msg_Dbg( p_stream, "decoder aspect is %f:1", f_aspect );
    /* Change f_aspect from source frame to source pixel */
    f_aspect = f_aspect * i_src_visible_height / i_src_visible_width;
    msg_Dbg( p_stream, "source pixel aspect is %f:1", f_aspect );
    /* Calculate scaling factor for specified parameters */
    if( id->p_encoder->fmt_out.video.i_visible_width <= 0 &&
        id->p_encoder->fmt_out.video.i_visible_height <= 0 && p_sys->f_scale )
    {
        /* Global scaling. Make sure width will remain a factor of 16 */
        float f_real_scale;
        int  i_new_height;
        int i_new_width = i_src_visible_width * p_sys->f_scale;
        if( i_new_width % 16 <= 7 && i_new_width >= 16 )
            i_new_width -= i_new_width % 16;
        else
            i_new_width += 16 - i_new_width % 16;
        f_real_scale = (float)( i_new_width ) / (float) i_src_visible_width;
        i_new_height = __MAX( 16, i_src_visible_height * (float)f_real_scale );
        f_scale_width = f_real_scale;
        f_scale_height = (float) i_new_height / (float) i_src_visible_height;
    }
    else if( id->p_encoder->fmt_out.video.i_visible_width > 0 &&
             id->p_encoder->fmt_out.video.i_visible_height <= 0 )
    {
        /* Only width specified */
        f_scale_width = (float)id->p_encoder->fmt_out.video.i_visible_width/i_src_visible_width;
        f_scale_height = f_scale_width;
    }
    else if( id->p_encoder->fmt_out.video.i_visible_width <= 0 &&
             id->p_encoder->fmt_out.video.i_visible_height > 0 )
    {
         /* Only height specified */
         f_scale_height = (float)id->p_encoder->fmt_out.video.i_visible_height/i_src_visible_height;
         f_scale_width = f_scale_height;
     }
     else if( id->p_encoder->fmt_out.video.i_visible_width > 0 &&
              id->p_encoder->fmt_out.video.i_visible_height > 0 )
     {
         /* Width and height specified */
         f_scale_width = (float)id->p_encoder->fmt_out.video.i_visible_width/i_src_visible_width;
         f_scale_height = (float)id->p_encoder->fmt_out.video.i_visible_height/i_src_visible_height;
     }
     /* check maxwidth and maxheight */
     if( p_sys->i_maxwidth && f_scale_width > (float)p_sys->i_maxwidth /
                                                     i_src_visible_width )
     {
         f_scale_width = (float)p_sys->i_maxwidth / i_src_visible_width;
     }
     if( p_sys->i_maxheight && f_scale_height > (float)p_sys->i_maxheight /
                                                       i_src_visible_height )
     {
         f_scale_height = (float)p_sys->i_maxheight / i_src_visible_height;
     }
     /* Change aspect ratio from source pixel to scaled pixel */
     f_aspect = f_aspect * f_scale_height / f_scale_width;
     msg_Dbg( p_stream, "scaled pixel aspect is %f:1", f_aspect );
     /* f_scale_width and f_scale_height are now final */
     /* Calculate width, height from scaling
      * Make sure its multiple of 2
      */
     /* width/height of output stream */
     int i_dst_visible_width =  2 * (int)(f_scale_width*i_src_visible_width/2+0.5);
     int i_dst_visible_height = 2 * (int)(f_scale_height*i_src_visible_height/2+0.5);
     int i_dst_width =  2 * (int)(f_scale_width*p_fmt_out->video.i_width/2+0.5);
     int i_dst_height = 2 * (int)(f_scale_height*p_fmt_out->video.i_height/2+0.5);
     /* Change aspect ratio from scaled pixel to output frame */
     f_aspect = f_aspect * i_dst_visible_width / i_dst_visible_height;
     /* Store calculated values */
     id->p_encoder->fmt_out.video.i_width = i_dst_width;
     id->p_encoder->fmt_out.video.i_visible_width = i_dst_visible_width;
     id->p_encoder->fmt_out.video.i_height = i_dst_height;
     id->p_encoder->fmt_out.video.i_visible_height = i_dst_visible_height;
     id->p_encoder->fmt_in.video.i_width = i_dst_width;
     id->p_encoder->fmt_in.video.i_visible_width = i_dst_visible_width;
     id->p_encoder->fmt_in.video.i_height = i_dst_height;
     id->p_encoder->fmt_in.video.i_visible_height = i_dst_visible_height;
     msg_Dbg( p_stream, "source %ix%i, destination %ix%i",
         i_src_visible_width, i_src_visible_height,
         i_dst_visible_width, i_dst_visible_height
     );
    /* Handle frame rate conversion */
    if( !id->p_encoder->fmt_out.video.i_frame_rate ||
        !id->p_encoder->fmt_out.video.i_frame_rate_base )
    {
        if( p_fmt_out->video.i_frame_rate &&
            p_fmt_out->video.i_frame_rate_base )
        {
            id->p_encoder->fmt_out.video.i_frame_rate =
                p_fmt_out->video.i_frame_rate;
            id->p_encoder->fmt_out.video.i_frame_rate_base =
                p_fmt_out->video.i_frame_rate_base;
        }
        else
        {
            /* Pick a sensible default value */
            id->p_encoder->fmt_out.video.i_frame_rate = ENC_FRAMERATE;
            id->p_encoder->fmt_out.video.i_frame_rate_base = ENC_FRAMERATE_BASE;
        }
    }
    id->p_encoder->fmt_in.video.orientation =
        id->p_encoder->fmt_out.video.orientation =
        id->p_decoder->fmt_in.video.orientation;
    id->p_encoder->fmt_in.video.i_frame_rate =
        id->p_encoder->fmt_out.video.i_frame_rate;
    id->p_encoder->fmt_in.video.i_frame_rate_base =
        id->p_encoder->fmt_out.video.i_frame_rate_base;
    vlc_ureduce( &id->p_encoder->fmt_in.video.i_frame_rate,
        &id->p_encoder->fmt_in.video.i_frame_rate_base,
        id->p_encoder->fmt_in.video.i_frame_rate,
        id->p_encoder->fmt_in.video.i_frame_rate_base,
        0 );
     msg_Dbg( p_stream, "source fps %d/%d, destination %d/%d",
        id->p_decoder->fmt_out.video.i_frame_rate,
        id->p_decoder->fmt_out.video.i_frame_rate_base,
        id->p_encoder->fmt_in.video.i_frame_rate,
        id->p_encoder->fmt_in.video.i_frame_rate_base );
    id->i_input_frame_interval  = id->p_decoder->fmt_out.video.i_frame_rate_base * CLOCK_FREQ / id->p_decoder->fmt_out.video.i_frame_rate;
    msg_Info( p_stream, "input interval %d (base %d)",
                        id->i_input_frame_interval, id->p_decoder->fmt_out.video.i_frame_rate_base );
    id->i_output_frame_interval = id->p_encoder->fmt_in.video.i_frame_rate_base * CLOCK_FREQ / id->p_encoder->fmt_in.video.i_frame_rate;
    msg_Info( p_stream, "output interval %d (base %d)",
                        id->i_output_frame_interval, id->p_encoder->fmt_in.video.i_frame_rate_base );
    date_Init( &id->next_input_pts,
               id->p_decoder->fmt_out.video.i_frame_rate,
               1 );
    date_Init( &id->next_output_pts,
               id->p_encoder->fmt_in.video.i_frame_rate,
               1 );
    /* Check whether a particular aspect ratio was requested */
    if( id->p_encoder->fmt_out.video.i_sar_num <= 0 ||
        id->p_encoder->fmt_out.video.i_sar_den <= 0 )
    {
        vlc_ureduce( &id->p_encoder->fmt_out.video.i_sar_num,
                     &id->p_encoder->fmt_out.video.i_sar_den,
                     (uint64_t)p_fmt_out->video.i_sar_num * i_src_visible_width  * i_dst_visible_height,
                     (uint64_t)p_fmt_out->video.i_sar_den * i_src_visible_height * i_dst_visible_width,
                     0 );
    }
    else
    {
        vlc_ureduce( &id->p_encoder->fmt_out.video.i_sar_num,
                     &id->p_encoder->fmt_out.video.i_sar_den,
                     id->p_encoder->fmt_out.video.i_sar_num,
                     id->p_encoder->fmt_out.video.i_sar_den,
                     0 );
    }
    id->p_encoder->fmt_in.video.i_sar_num =
        id->p_encoder->fmt_out.video.i_sar_num;
    id->p_encoder->fmt_in.video.i_sar_den =
        id->p_encoder->fmt_out.video.i_sar_den;
    msg_Dbg( p_stream, "encoder aspect is %i:%i",
             id->p_encoder->fmt_out.video.i_sar_num * id->p_encoder->fmt_out.video.i_width,
             id->p_encoder->fmt_out.video.i_sar_den * id->p_encoder->fmt_out.video.i_height );
}
