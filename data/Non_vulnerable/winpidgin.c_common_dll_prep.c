}
static BOOL common_dll_prep(const wchar_t *path) {
	HMODULE hmod;
	HKEY hkey;
	struct _stat stat_buf;
	wchar_t test_path[MAX_PATH + 1];
	_snwprintf(test_path, sizeof(test_path) / sizeof(wchar_t),
		L"%s\\libgtk-win32-2.0-0.dll", path);
	test_path[sizeof(test_path) / sizeof(wchar_t) - 1] = L'\0';
	if (_wstat(test_path, &stat_buf) != 0) {
		printf("Unable to determine GTK+ path. \n"
			"Assuming GTK+ is in the PATH.\n");
		return FALSE;
	}
	wprintf(L"GTK+ path found: %s\n", path);
	if ((hmod = GetModuleHandleW(L"kernel32.dll"))) {
		MySetDllDirectory = (LPFNSETDLLDIRECTORY) GetProcAddress(
			hmod, "SetDllDirectoryW");
		if (!MySetDllDirectory)
			printf("SetDllDirectory not supported\n");
	} else
		printf("Error getting kernel32.dll module handle\n");
	/* For Windows XP SP1+ / Server 2003 we use SetDllDirectory to avoid dll hell */
	if (MySetDllDirectory) {
		printf("Using SetDllDirectory\n");
		MySetDllDirectory(path);
	}
	/* For the rest, we set the current directory and make sure
	 * SafeDllSearch is set to 0 where needed. */
	else {
		OSVERSIONINFOW osinfo;
		printf("Setting current directory to GTK+ dll directory\n");
		SetCurrentDirectoryW(path);
		/* For Windows 2000 (SP3+) / WinXP (No SP):
		 * If SafeDllSearchMode is set to 1, Windows system directories are
		 * searched for dlls before the current directory. Therefore we set it
		 * to 0.
		 */
		osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
		GetVersionExW(&osinfo);
		if ((osinfo.dwMajorVersion == 5
				&& osinfo.dwMinorVersion == 0
				&& wcscmp(osinfo.szCSDVersion, L"Service Pack 3") >= 0)
			||
			(osinfo.dwMajorVersion == 5
				&& osinfo.dwMinorVersion == 1
				&& wcscmp(osinfo.szCSDVersion, L"") >= 0)
		) {
			DWORD regval = 1;
			DWORD reglen = sizeof(DWORD);
			printf("Using Win2k (SP3+) / WinXP (No SP)... Checking SafeDllSearch\n");
			read_reg_string(HKEY_LOCAL_MACHINE,
				L"System\\CurrentControlSet\\Control\\Session Manager",
				L"SafeDllSearchMode",
				(LPBYTE) &regval,
				&reglen);
			if (regval != 0) {
				printf("Trying to set SafeDllSearchMode to 0\n");
				regval = 0;
				if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
					L"System\\CurrentControlSet\\Control\\Session Manager",
					0,  KEY_SET_VALUE, &hkey
				) == ERROR_SUCCESS) {
					if (RegSetValueExW(hkey,
						L"SafeDllSearchMode", 0,
						REG_DWORD, (LPBYTE) &regval,
						sizeof(DWORD)
					) != ERROR_SUCCESS)
						printf("Error writing SafeDllSearchMode. Error: %u\n",
							(UINT) GetLastError());
					RegCloseKey(hkey);
				} else
					printf("Error opening Session Manager key for writing. Error: %u\n",
						(UINT) GetLastError());
			} else
				printf("SafeDllSearchMode is set to 0\n");
		}/*end else*/
	}
	return TRUE;
}
