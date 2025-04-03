 *****************************************************************************/
static void Stop( audio_output_t *p_aout )
{
    aout_sys_t *p_sys = p_aout->sys;
    /* Before calling waveOutClose we must reset the device */
    MMRESULT result = waveOutReset( p_sys->h_waveout );
    if(result != MMSYSERR_NOERROR)
    {
       msg_Err( p_aout, "waveOutReset failed 0x%x", result );
       /*
        now we must wait, that all buffers are played
        because cancel doesn't work in this case...
       */
       if(result == MMSYSERR_NOTSUPPORTED)
       {
           /*
             clear currently played (done) buffers,
             if returnvalue > 0 (means some buffer still playing)
             wait for the driver event callback that one buffer
             is finished with playing, and check again
             the timeout of 5000ms is just, an emergency exit
             of this loop, to avoid deadlock in case of other
             (currently not known bugs, problems, errors cases?)
           */
            WaveOutFlush( p_aout, true );
       }
    }
    /* wait for the frames to be queued in cleaning list */
    WaveOutFlush( p_aout, true );
    WaveOutClean( p_aout->sys );
    /* now we can Close the device */
    if( waveOutClose( p_sys->h_waveout ) != MMSYSERR_NOERROR )
    {
        msg_Err( p_aout, "waveOutClose failed" );
    }
    free( p_sys->p_silence_buffer );
    p_aout->sys->i_played_length = 0;
    p_sys->b_soft = true;
}
