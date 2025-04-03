 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    encoder_t     *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    free( p_sys->psz_stat_name );
    free( p_sys->p_sei );
    if( p_sys->h )
    {
        msg_Dbg( p_enc, "framecount still in libx264 buffer: %d", x264_encoder_delayed_frames( p_sys->h ) );
        x264_encoder_close( p_sys->h );
    }
#ifdef PTW32_STATIC_LIB
    vlc_mutex_lock( &pthread_win32_mutex );
    pthread_win32_count--;
    if( pthread_win32_count == 0 )
    {
        pthread_win32_thread_detach_np();
        pthread_win32_process_detach_np();
        msg_Dbg( p_enc, "pthread-win32 deinitialized" );
    }
    vlc_mutex_unlock( &pthread_win32_mutex );
#endif
    free( p_sys );
}
