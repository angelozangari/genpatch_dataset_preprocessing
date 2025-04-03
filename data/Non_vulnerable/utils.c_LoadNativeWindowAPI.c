#include <dlfcn.h>
void *LoadNativeWindowAPI(native_window_api_t *native)
{
    void *p_library = dlopen("libandroid.so", RTLD_NOW);
    if (!p_library)
        return NULL;
    native->winFromSurface =
        (ptr_ANativeWindow_fromSurface)(dlsym(p_library, "ANativeWindow_fromSurface"));
    native->winRelease =
        (ptr_ANativeWindow_release)(dlsym(p_library, "ANativeWindow_release"));
    native->winLock =
        (ptr_ANativeWindow_lock)(dlsym(p_library, "ANativeWindow_lock"));
    native->unlockAndPost =
        (ptr_ANativeWindow_unlockAndPost)(dlsym(p_library, "ANativeWindow_unlockAndPost"));
    if (native->winFromSurface && native->winRelease && native->winLock && native->unlockAndPost)
        return p_library;
    native->winFromSurface = NULL;
    native->winRelease = NULL;
    native->winLock = NULL;
    native->unlockAndPost = NULL;
    dlclose(p_library);
    return NULL;
}
