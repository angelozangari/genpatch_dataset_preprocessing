}
void CloseGestures( win32_gesture_sys_t *p_gesture )
{
    if (p_gesture && p_gesture->huser_dll )
        FreeLibrary( p_gesture->huser_dll );
    free( p_gesture );
}
