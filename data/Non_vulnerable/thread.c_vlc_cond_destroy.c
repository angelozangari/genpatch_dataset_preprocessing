}
void vlc_cond_destroy (vlc_cond_t *p_condvar)
{
    CloseHandle (p_condvar->handle);
}
