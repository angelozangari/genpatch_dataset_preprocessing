}
static int Open( vlc_object_t *obj )
{
    filter_t *p_filter  = (filter_t *)obj;
    filter_sys_t *p_sys;
    if( p_filter->fmt_in.audio.i_format != VLC_CODEC_FL32 ||
     !AOUT_FMTS_IDENTICAL( &p_filter->fmt_in.audio, &p_filter->fmt_out.audio) )
        return VLC_EGENERIC;
    if( p_filter->fmt_in.audio.i_channels != 2 )
    {
        msg_Err ( p_filter, "stereo enhance requires stereo" );
        return VLC_EGENERIC;
    }
    p_sys = p_filter->p_sys = malloc( sizeof(filter_sys_t) );
    if( unlikely(!p_sys) )
        return VLC_ENOMEM;
#define CREATE_VAR( stor, var ) \
    p_sys->stor = var_CreateGetFloat( obj, var ); \
    var_AddCallback( p_filter, var, paramCallback, p_sys );
    CREATE_VAR( f_delay, "delay" )
    CREATE_VAR( f_feedback, "feedback" )
    CREATE_VAR( f_crossfeed, "crossfeed" )
    CREATE_VAR( f_dry_mix, "dry-mix" )
    /* Compute buffer length and allocate space */
    p_sys->pf_ringbuf = NULL;
    p_sys->i_len = 0;
    if( MakeRingBuffer( &p_sys->pf_ringbuf, &p_sys->i_len, &p_sys->pf_write,
                        p_sys->f_delay, p_filter->fmt_in.audio.i_rate ) != VLC_SUCCESS )
    {
        free( p_sys );
        return VLC_ENOMEM;
    }
    p_filter->pf_audio_filter = Filter;
    return VLC_SUCCESS;
}
