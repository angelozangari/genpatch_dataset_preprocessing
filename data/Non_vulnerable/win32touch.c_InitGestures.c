}
BOOL InitGestures( HWND hwnd, win32_gesture_sys_t **pp_gesture )
{
    BOOL result = FALSE;
    GESTURECONFIG config = { 0, 0, 0 };
    config.dwID    = GID_PAN;
    config.dwWant  = GC_PAN |
                     GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY |
                     GC_PAN_WITH_SINGLE_FINGER_VERTICALLY;
    config.dwBlock = GC_PAN_WITH_INERTIA;
    win32_gesture_sys_t *p_gesture = malloc( sizeof(win32_gesture_sys_t) );
    if( !p_gesture )
    {
        *pp_gesture = NULL;
        return FALSE;
    }
    HINSTANCE h_user32_dll = LoadLibrary(TEXT("user32.dll"));
    if( !h_user32_dll )
    {
        *pp_gesture = NULL;
        free( p_gesture );
        return FALSE;
    }
    BOOL (WINAPI *OurSetGestureConfig) (HWND, DWORD, UINT, PGESTURECONFIG, UINT);
    OurSetGestureConfig = (void *)GetProcAddress(h_user32_dll, "SetGestureConfig");
    p_gesture->OurCloseGestureInfoHandle =
        (void *)GetProcAddress(h_user32_dll, "CloseGestureInfoHandle" );
    p_gesture->OurGetGestureInfo =
        (void *)GetProcAddress(h_user32_dll, "GetGestureInfo");
    if( OurSetGestureConfig )
    {
        result = OurSetGestureConfig(
                hwnd,
                0,
                1,
                &config,
                sizeof( GESTURECONFIG )
                );
    }
    p_gesture->i_type     = 0;
    p_gesture->b_2fingers = false;
    p_gesture->i_action   = GESTURE_ACTION_UNDEFINED;
    p_gesture->i_beginx   = p_gesture->i_beginy = -1;
    p_gesture->i_lasty    = -1;
    p_gesture->huser_dll  = h_user32_dll;
    *pp_gesture = p_gesture;
    return result;
}
