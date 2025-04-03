/* APC procedure for thread cancellation */
static void CALLBACK vlc_cancel_self (ULONG_PTR self)
{
    struct vlc_thread *th = (void *)self;
    if (likely(th != NULL))
        th->killed = true;
}
