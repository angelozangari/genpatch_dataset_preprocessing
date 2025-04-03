static vlc_mutex_t single_instance = VLC_STATIC_MUTEX;
static inline void *LoadSurface(const char *psz_lib, vout_display_sys_t *sys)
{
    void *p_library = dlopen(psz_lib, RTLD_NOW);
    if (!p_library)
        return NULL;
    sys->s_lock = (Surface_lock)(dlsym(p_library, ANDROID_SYM_S_LOCK));
    sys->s_lock2 = (Surface_lock2)(dlsym(p_library, ANDROID_SYM_S_LOCK2));
    sys->s_unlockAndPost =
        (Surface_unlockAndPost)(dlsym(p_library, ANDROID_SYM_S_UNLOCK));
    if ((sys->s_lock || sys->s_lock2) && sys->s_unlockAndPost)
        return p_library;
    dlclose(p_library);
    return NULL;
}
