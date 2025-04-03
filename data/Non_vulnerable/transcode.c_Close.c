 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_stream_t       *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t   *p_sys = p_stream->p_sys;
    free( p_sys->psz_af );
    config_ChainDestroy( p_sys->p_audio_cfg );
    free( p_sys->psz_aenc );
    free( p_sys->psz_alang );
    free( p_sys->psz_vf2 );
    config_ChainDestroy( p_sys->p_video_cfg );
    free( p_sys->psz_venc );
    config_ChainDestroy( p_sys->p_deinterlace_cfg );
    free( p_sys->psz_deinterlace );
    config_ChainDestroy( p_sys->p_spu_cfg );
    free( p_sys->psz_senc );
    if( p_sys->p_spu ) spu_Destroy( p_sys->p_spu );
    if( p_sys->p_spu_blend ) filter_DeleteBlend( p_sys->p_spu_blend );
    config_ChainDestroy( p_sys->p_osd_cfg );
    free( p_sys->psz_osdenc );
    free( p_sys );
}
