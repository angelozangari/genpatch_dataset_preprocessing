}
static void WaveOutClean( aout_sys_t * p_sys )
{
    struct lkwavehdr *p_whdr, *p_list;
    vlc_mutex_lock(&p_sys->lock);
    p_list =  p_sys->p_free_list;
    p_sys->p_free_list = NULL;
    vlc_mutex_unlock(&p_sys->lock);
    while( p_list )
    {
        p_whdr = p_list;
        p_list = p_list->p_next;
        WaveOutClearBuffer( p_sys->h_waveout, &p_whdr->hdr );
        free(p_whdr);
    }
}
