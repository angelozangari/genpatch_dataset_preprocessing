}
static void WaveOutClearBuffer( HWAVEOUT h_waveout, WAVEHDR *p_waveheader )
{
    block_t *p_buffer = (block_t *)(p_waveheader->dwUser);
    /* Unprepare and free the buffers which has just been played */
    waveOutUnprepareHeader( h_waveout, p_waveheader, sizeof(WAVEHDR) );
    if( p_waveheader->dwUser != 1 )
        block_Release( p_buffer );
}
