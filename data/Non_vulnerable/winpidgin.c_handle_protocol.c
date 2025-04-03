#define PROTO_HANDLER_SWITCH L"--protocolhandler="
static void handle_protocol(wchar_t *cmd) {
	char *remote_msg, *utf8msg;
	wchar_t *tmp1, *tmp2;
	int len, wlen;
	SIZE_T len_written;
	HWND msg_win;
	DWORD pid;
	HANDLE process;
	/* The start of the message */
	tmp1 = cmd + wcslen(PROTO_HANDLER_SWITCH);
	/* The end of the message */
	if ((tmp2 = wcschr(tmp1, L' ')))
		wlen = (tmp2 - tmp1);
	else
		wlen = wcslen(tmp1);
	if (wlen == 0) {
		printf("No protocol message specified.\n");
		return;
	}
	if (!(msg_win = FindWindowExW(NULL, NULL, L"WinpidginMsgWinCls", NULL))) {
		printf("Unable to find an instance of Pidgin to handle protocol message.\n");
		return;
	}
	len = WideCharToMultiByte(CP_UTF8, 0, tmp1,
			wlen, NULL, 0, NULL, NULL);
	if (len) {
		utf8msg = malloc(len);
		len = WideCharToMultiByte(CP_UTF8, 0, tmp1,
			wlen, utf8msg, len, NULL, NULL);
	}
	if (len == 0) {
		printf("No protocol message specified.\n");
		return;
	}
	GetWindowThreadProcessId(msg_win, &pid);
	if (!(process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid))) {
		DWORD dw = GetLastError();
		const wchar_t *err_msg = get_win32_error_message(dw);
		wprintf(L"Unable to open Pidgin process. (%u) %s\n", (UINT) dw, err_msg);
		return;
	}
	wprintf(L"Trying to handle protocol message:\n'%.*s'\n", wlen, tmp1);
	/* MEM_COMMIT initializes the memory to zero
	 * so we don't need to worry that our section of utf8msg isn't nul-terminated */
	if ((remote_msg = (char*) VirtualAllocEx(process, NULL, len + 1, MEM_COMMIT, PAGE_READWRITE))) {
		if (WriteProcessMemory(process, remote_msg, utf8msg, len, &len_written)) {
			if (!SendMessageA(msg_win, PIDGIN_WM_PROTOCOL_HANDLE, len_written, (LPARAM) remote_msg))
				printf("Unable to send protocol message to Pidgin instance.\n");
		} else {
			DWORD dw = GetLastError();
			const wchar_t *err_msg = get_win32_error_message(dw);
			wprintf(L"Unable to write to remote memory. (%u) %s\n", (UINT) dw, err_msg);
		}
		VirtualFreeEx(process, remote_msg, 0, MEM_RELEASE);
	} else {
		DWORD dw = GetLastError();
		const wchar_t *err_msg = get_win32_error_message(dw);
		wprintf(L"Unable to allocate remote memory. (%u) %s\n", (UINT) dw, err_msg);
	}
	CloseHandle(process);
	free(utf8msg);
}
