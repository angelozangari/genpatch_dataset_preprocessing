BOOL WINAPI DllMain (HINSTANCE, DWORD, LPVOID);
BOOL WINAPI DllMain (HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    (void) hinstDll;
    (void) lpvReserved;
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            InitializeCriticalSection (&clock_lock);
            vlc_mutex_init (&super_mutex);
            vlc_cond_init (&super_variable);
            vlc_threadvar_create (&thread_key, NULL);
            vlc_rwlock_init (&config_lock);
            vlc_CPU_init ();
            break;
        case DLL_PROCESS_DETACH:
            vlc_rwlock_destroy (&config_lock);
            vlc_threadvar_delete (&thread_key);
            vlc_cond_destroy (&super_variable);
            vlc_mutex_destroy (&super_mutex);
            DeleteCriticalSection (&clock_lock);
            break;
    }
    return TRUE;
}
