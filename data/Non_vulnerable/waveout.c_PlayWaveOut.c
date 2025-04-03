 *****************************************************************************/
static int PlayWaveOut( audio_output_t *p_aout, HWAVEOUT h_waveout,
                        struct lkwavehdr *p_waveheader, block_t *p_buffer, bool b_spdif)
{
    MMRESULT result;
    /* Prepare the buffer */
    if( p_buffer != NULL )
    {
        p_waveheader->hdr.lpData = (LPSTR)p_buffer->p_buffer;
        p_waveheader->hdr.dwBufferLength = p_buffer->i_buffer;
        /*
          copy the buffer to the silence buffer :) so in case we don't
          get the next buffer fast enough (I will repeat this one a time
          for AC3 / DTS and SPDIF this will sound better instead of
          a hickup)
        */
        if(b_spdif)
        {
           memcpy( p_aout->sys->p_silence_buffer,
                       p_buffer->p_buffer,
                       p_aout->sys->i_buffer_size );
           p_aout->sys->i_repeat_counter = 2;
        }
    } else {
        /* Use silence buffer instead */
        if(p_aout->sys->i_repeat_counter)
        {
           p_aout->sys->i_repeat_counter--;
           if(!p_aout->sys->i_repeat_counter)
           {
               memset( p_aout->sys->p_silence_buffer,
                           0x00, p_aout->sys->i_buffer_size );
           }
        }
        p_waveheader->hdr.lpData = (LPSTR)p_aout->sys->p_silence_buffer;
        p_waveheader->hdr.dwBufferLength = p_aout->sys->i_buffer_size;
    }
    p_waveheader->hdr.dwUser = p_buffer ? (DWORD_PTR)p_buffer : (DWORD_PTR)1;
    p_waveheader->hdr.dwFlags = 0;
    result = waveOutPrepareHeader( h_waveout, &p_waveheader->hdr, sizeof(WAVEHDR) );
    if( result != MMSYSERR_NOERROR )
    {
        msg_Err( p_aout, "waveOutPrepareHeader failed" );
        return VLC_EGENERIC;
    }
    /* Send the buffer to the waveOut queue */
    result = waveOutWrite( h_waveout, &p_waveheader->hdr, sizeof(WAVEHDR) );
    if( result != MMSYSERR_NOERROR )
    {
        msg_Err( p_aout, "waveOutWrite failed" );
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
