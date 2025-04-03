}
static void *HelperThread(void *data)
{
    intf_thread_t *intf = data;
    intf_sys_t *sys = intf->p_sys;
    HWND ipcwindow =
        CreateWindow(L"STATIC",                      /* name of window class */
                  L"VLC ipc " TEXT(VERSION),        /* window title bar text */
                  0,                                         /* window style */
                  0,                                 /* default X coordinate */
                  0,                                 /* default Y coordinate */
                  0,                                         /* window width */
                  0,                                        /* window height */
                  NULL,                                  /* no parent window */
                  NULL,                            /* no menu in this window */
                  GetModuleHandle(NULL),  /* handle of this program instance */
                  NULL) ;                               /* sent to WM_CREATE */
    SetWindowLongPtr(ipcwindow, GWLP_WNDPROC, (LRESULT)WMCOPYWNDPROC);
    SetWindowLongPtr(ipcwindow, GWLP_USERDATA, (uintptr_t)data);
    sys->window = ipcwindow;
    /* Signal the creation of the thread and events queue */
    SetEvent(sys->ready);
    MSG message;
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    return NULL;
}
