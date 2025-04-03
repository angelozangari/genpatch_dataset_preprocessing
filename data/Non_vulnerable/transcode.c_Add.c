}
static sout_stream_id_sys_t *Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    sout_stream_id_sys_t *id;
    id = calloc( 1, sizeof( sout_stream_id_sys_t ) );
    if( !id )
        goto error;
    id->id = NULL;
    id->p_decoder = NULL;
    id->p_encoder = NULL;
    /* Create decoder object */
    id->p_decoder = vlc_object_create( p_stream, sizeof( decoder_t ) );
    if( !id->p_decoder )
        goto error;
    id->p_decoder->p_module = NULL;
    id->p_decoder->fmt_in = *p_fmt;
    id->p_decoder->b_pace_control = true;
    /* Create encoder object */
    id->p_encoder = sout_EncoderCreate( p_stream );
    if( !id->p_encoder )
        goto error;
    id->p_encoder->p_module = NULL;
    /* Create destination format */
    es_format_Init( &id->p_encoder->fmt_out, p_fmt->i_cat, 0 );
    id->p_encoder->fmt_out.i_id    = p_fmt->i_id;
    id->p_encoder->fmt_out.i_group = p_fmt->i_group;
    if( p_sys->psz_alang )
        id->p_encoder->fmt_out.psz_language = strdup( p_sys->psz_alang );
    else if( p_fmt->psz_language )
        id->p_encoder->fmt_out.psz_language = strdup( p_fmt->psz_language );
    bool success;
    if( p_fmt->i_cat == AUDIO_ES && p_sys->i_acodec )
        success = transcode_audio_add(p_stream, p_fmt, id);
    else if( p_fmt->i_cat == VIDEO_ES && p_sys->i_vcodec )
        success = transcode_video_add(p_stream, p_fmt, id);
    else if( ( p_fmt->i_cat == SPU_ES ) &&
             ( p_sys->i_scodec || p_sys->b_soverlay ) )
        success = transcode_spu_add(p_stream, p_fmt, id);
    else if( !p_sys->b_osd && (p_sys->i_osdcodec != 0 || p_sys->psz_osdenc) )
        success = transcode_osd_add(p_stream, p_fmt, id);
    else
    {
        msg_Dbg( p_stream, "not transcoding a stream (fcc=`%4.4s')",
                 (char*)&p_fmt->i_codec );
        id->id = sout_StreamIdAdd( p_stream->p_next, p_fmt );
        id->b_transcode = false;
        success = id->id;
    }
    if(!success)
        goto error;
    return id;
error:
    if( id )
    {
        if( id->p_decoder )
        {
            vlc_object_release( id->p_decoder );
            id->p_decoder = NULL;
        }
        if( id->p_encoder )
        {
            es_format_Clean( &id->p_encoder->fmt_out );
            vlc_object_release( id->p_encoder );
            id->p_encoder = NULL;
        }
        free( id );
    }
    return NULL;
}
