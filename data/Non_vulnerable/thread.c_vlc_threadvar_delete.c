}
void vlc_threadvar_delete (vlc_threadvar_t *p_tls)
{
    struct vlc_threadvar *var = *p_tls;
    vlc_mutex_lock (&super_mutex);
    if (var->prev != NULL)
        var->prev->next = var->next;
    if (var->next != NULL)
        var->next->prev = var->prev;
    else
        vlc_threadvar_last = var->prev;
    vlc_mutex_unlock (&super_mutex);
    TlsFree (var->id);
    free (var);
}
