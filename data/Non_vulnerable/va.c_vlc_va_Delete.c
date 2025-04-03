}
void vlc_va_Delete(vlc_va_t *va)
{
    vlc_module_unload(va->module, vlc_va_Stop, va);
    vlc_object_release(va);
}
