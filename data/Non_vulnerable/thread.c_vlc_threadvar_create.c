} *vlc_threadvar_last = NULL;
int vlc_threadvar_create (vlc_threadvar_t *p_tls, void (*destr) (void *))
{
    struct vlc_threadvar *var = malloc (sizeof (*var));
    if (unlikely(var == NULL))
        return errno;
    var->id = TlsAlloc();
    if (var->id == TLS_OUT_OF_INDEXES)
    {
        free (var);
        return EAGAIN;
    }
    var->destroy = destr;
    var->next = NULL;
    *p_tls = var;
    vlc_mutex_lock (&super_mutex);
    var->prev = vlc_threadvar_last;
    if (var->prev)
        var->prev->next = var;
    vlc_threadvar_last = var;
    vlc_mutex_unlock (&super_mutex);
    return 0;
}
