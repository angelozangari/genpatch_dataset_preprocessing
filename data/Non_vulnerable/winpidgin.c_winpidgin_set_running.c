#define PIDGIN_WM_PROTOCOL_HANDLE (WM_APP + 14)
static BOOL winpidgin_set_running(BOOL fail_if_running) {
	HANDLE h;
	if ((h = CreateMutexW(NULL, FALSE, L"pidgin_is_running"))) {
		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS) {
			if (fail_if_running) {
				HWND msg_win;
				printf("An instance of Pidgin is already running.\n");
				if((msg_win = FindWindowExW(NULL, NULL, L"WinpidginMsgWinCls", NULL)))
					if(SendMessage(msg_win, PIDGIN_WM_FOCUS_REQUEST, (WPARAM) NULL, (LPARAM) NULL))
						return FALSE;
				/* If we get here, the focus request wasn't successful */
				MessageBoxW(NULL,
					L"An instance of Pidgin is already running",
					NULL, MB_OK | MB_TOPMOST);
				return FALSE;
			}
		} else if (err != ERROR_SUCCESS)
			printf("Error (%u) accessing \"pidgin_is_running\" mutex.\n", (UINT) err);
	}
	return TRUE;
}
#define PIDGIN_WM_PROTOCOL_HANDLE (WM_APP + 14)
static BOOL winpidgin_set_running(BOOL fail_if_running) {
	HANDLE h;
	if ((h = CreateMutexW(NULL, FALSE, L"pidgin_is_running"))) {
		DWORD err = GetLastError();
		if (err == ERROR_ALREADY_EXISTS) {
			if (fail_if_running) {
				HWND msg_win;
				printf("An instance of Pidgin is already running.\n");
				if((msg_win = FindWindowExW(NULL, NULL, L"WinpidginMsgWinCls", NULL)))
					if(SendMessage(msg_win, PIDGIN_WM_FOCUS_REQUEST, (WPARAM) NULL, (LPARAM) NULL))
						return FALSE;
				/* If we get here, the focus request wasn't successful */
				MessageBoxW(NULL,
					L"An instance of Pidgin is already running",
					NULL, MB_OK | MB_TOPMOST);
				return FALSE;
			}
		} else if (err != ERROR_SUCCESS)
			printf("Error (%u) accessing \"pidgin_is_running\" mutex.\n", (UINT) err);
	}
