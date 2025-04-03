 *****************************************************************************/
static void CALLBACK WaveOutCallback( HWAVEOUT h_waveout, UINT uMsg,
                                      DWORD_PTR _p_aout,
                                      DWORD_PTR dwParam1, DWORD_PTR dwParam2 )
{
    (void) h_waveout;
    (void) dwParam2;
    audio_output_t *p_aout = (audio_output_t *)_p_aout;
    struct lkwavehdr * p_waveheader =  (struct lkwavehdr *) dwParam1;
    if( uMsg != WOM_DONE ) return;
    vlc_mutex_lock( &p_aout->sys->lock );
    p_waveheader->p_next = p_aout->sys->p_free_list;
    p_aout->sys->p_free_list = p_waveheader;
    p_aout->sys->i_frames--;
    vlc_cond_broadcast( &p_aout->sys->cond );
    vlc_mutex_unlock( &p_aout->sys->lock );
}
