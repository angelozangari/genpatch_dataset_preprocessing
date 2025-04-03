}
static void *InitLibrary(vout_display_sys_t *sys)
{
    static const char *libs[] = {
        "libsurfaceflinger_client.so",
        "libgui.so",
        "libui.so"
    };
    for (size_t i = 0; i < sizeof(libs) / sizeof(*libs); i++) {
        void *lib = LoadSurface(libs[i], sys);
        if (lib)
            return lib;
    }
    return NULL;
}
