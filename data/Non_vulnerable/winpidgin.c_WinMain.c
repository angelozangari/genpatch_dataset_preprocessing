int _stdcall
WinMain (struct HINSTANCE__ *hInstance, struct HINSTANCE__ *hPrevInstance,
		char *lpszCmdLine, int nCmdShow) {
	wchar_t errbuf[512];
	wchar_t pidgin_dir[MAX_PATH];
	wchar_t *pidgin_dir_start = NULL;
	wchar_t exe_name[MAX_PATH];
	HMODULE hmod;
	wchar_t *wtmp;
	int pidgin_argc;
	char **pidgin_argv; /* This is in utf-8 */
	int i, j, k;
	BOOL debug = FALSE, help = FALSE, version = FALSE, multiple = FALSE, success;
	LPWSTR *szArglist;
	LPWSTR cmdLine;
	/* If debug or help or version flag used, create console for output */
	for (i = 1; i < __argc; i++) {
		if (strlen(__argv[i]) > 1 && __argv[i][0] == '-') {
			/* check if we're looking at -- or - option */
			if (__argv[i][1] == '-') {
				if (strstr(__argv[i], "--debug") == __argv[i])
					debug = TRUE;
				else if (strstr(__argv[i], "--help") == __argv[i])
					help = TRUE;
				else if (strstr(__argv[i], "--version") == __argv[i])
					version = TRUE;
				else if (strstr(__argv[i], "--multiple") == __argv[i])
					multiple = TRUE;
			} else {
				if (strchr(__argv[i], 'd'))
					debug = TRUE;
				if (strchr(__argv[i], 'h'))
					help = TRUE;
				if (strchr(__argv[i], 'v'))
					version = TRUE;
				if (strchr(__argv[i], 'm'))
					multiple = TRUE;
			}
		}
	}
	/* Permanently enable DEP if the OS supports it */
	if ((hmod = GetModuleHandleW(L"kernel32.dll"))) {
		LPFNSETPROCESSDEPPOLICY MySetProcessDEPPolicy =
			(LPFNSETPROCESSDEPPOLICY)
			GetProcAddress(hmod, "SetProcessDEPPolicy");
		if (MySetProcessDEPPolicy)
			MySetProcessDEPPolicy(1); //PROCESS_DEP_ENABLE
	}
	if (debug || help || version) {
		/* If stdout hasn't been redirected to a file, alloc a console
		 *  (_istty() doesn't work for stuff using the GUI subsystem) */
		if (_fileno(stdout) == -1 || _fileno(stdout) == -2) {
			LPFNATTACHCONSOLE MyAttachConsole = NULL;
			if (hmod)
				MyAttachConsole =
					(LPFNATTACHCONSOLE)
					GetProcAddress(hmod, "AttachConsole");
			if ((MyAttachConsole && MyAttachConsole(ATTACH_PARENT_PROCESS))
					|| AllocConsole()) {
				freopen("CONOUT$", "w", stdout);
				freopen("CONOUT$", "w", stderr);
			}
		}
	}
	cmdLine = GetCommandLineW();
	/* If this is a protocol handler invocation, deal with it accordingly */
	if ((wtmp = wcsstr(cmdLine, PROTO_HANDLER_SWITCH)) != NULL) {
		handle_protocol(wtmp);
		return 0;
	}
	/* Load exception handler if we have it */
	if (GetModuleFileNameW(NULL, pidgin_dir, MAX_PATH) != 0) {
		/* primitive dirname() */
		pidgin_dir_start = wcsrchr(pidgin_dir, L'\\');
		if (pidgin_dir_start) {
			HMODULE hmod;
			pidgin_dir_start[0] = L'\0';
			/* tmp++ will now point to the executable file name */
			wcscpy(exe_name, pidgin_dir_start + 1);
			wcscat(pidgin_dir, L"\\exchndl.dll");
			if ((hmod = LoadLibraryW(pidgin_dir))) {
				typedef void (__cdecl* LPFNSETLOGFILE)(const LPCSTR);
				LPFNSETLOGFILE MySetLogFile;
				/* exchndl.dll is built without UNICODE */
				char debug_dir[MAX_PATH];
				printf("Loaded exchndl.dll\n");
				/* Temporarily override exchndl.dll's logfile
				 * to something sane (Pidgin will override it
				 * again when it initializes) */
				MySetLogFile = (LPFNSETLOGFILE) GetProcAddress(hmod, "SetLogFile");
				if (MySetLogFile) {
					if (GetTempPathA(sizeof(debug_dir), debug_dir) != 0) {
						strcat(debug_dir, "pidgin.RPT");
						printf(" Setting exchndl.dll LogFile to %s\n",
							debug_dir);
						MySetLogFile(debug_dir);
					}
				}
				/* The function signature for SetDebugInfoDir is the same as SetLogFile,
				 * so we can reuse the variable */
				MySetLogFile = (LPFNSETLOGFILE) GetProcAddress(hmod, "SetDebugInfoDir");
				if (MySetLogFile) {
					char *pidgin_dir_ansi = NULL;
					/* Restore pidgin_dir to point to where the executable is */
					pidgin_dir_start[0] = L'\0';
					i = WideCharToMultiByte(CP_ACP, 0, pidgin_dir,
						-1, NULL, 0, NULL, NULL);
					if (i != 0) {
						pidgin_dir_ansi = malloc(i);
						i = WideCharToMultiByte(CP_ACP, 0, pidgin_dir,
							-1, pidgin_dir_ansi, i, NULL, NULL);
						if (i == 0) {
							free(pidgin_dir_ansi);
							pidgin_dir_ansi = NULL;
						}
					}
					if (pidgin_dir_ansi != NULL) {
						_snprintf(debug_dir, sizeof(debug_dir),
							"%s\\pidgin-%s-dbgsym",
							pidgin_dir_ansi,  VERSION);
						debug_dir[sizeof(debug_dir) - 1] = '\0';
						printf(" Setting exchndl.dll DebugInfoDir to %s\n",
							debug_dir);
						MySetLogFile(debug_dir);
						free(pidgin_dir_ansi);
					}
				}
			}
			/* Restore pidgin_dir to point to where the executable is */
			pidgin_dir_start[0] = L'\0';
		}
	} else {
		DWORD dw = GetLastError();
		const wchar_t *err_msg = get_win32_error_message(dw);
		_snwprintf(errbuf, 512,
			L"Error getting module filename.\nError: (%u) %s",
			(UINT) dw, err_msg);
		wprintf(L"%s\n", errbuf);
		MessageBoxW(NULL, errbuf, NULL, MB_OK | MB_TOPMOST);
		pidgin_dir[0] = L'\0';
	}
	/* Determine if we're running in portable mode */
	if (wcsstr(cmdLine, L"--portable-mode")
			|| (exe_name != NULL && wcsstr(exe_name, L"-portable.exe"))) {
		printf("Running in PORTABLE mode.\n");
		portable_mode = TRUE;
	}
	if (portable_mode)
		portable_mode_dll_prep(pidgin_dir);
	else if (!getenv("PIDGIN_NO_DLL_CHECK"))
		dll_prep(pidgin_dir);
	winpidgin_set_locale();
	/* If help, version or multiple flag used, do not check Mutex */
	if (!help && !version)
		if (!winpidgin_set_running(getenv("PIDGIN_MULTI_INST") == NULL && !multiple))
			return 0;
	/* Now we are ready for Pidgin .. */
	wcscat(pidgin_dir, L"\\pidgin.dll");
	if ((hmod = LoadLibraryW(pidgin_dir)))
		pidgin_main = (LPFNPIDGINMAIN) GetProcAddress(hmod, "pidgin_main");
	/* Restore pidgin_dir to point to where the executable is */
	if (pidgin_dir_start)
		pidgin_dir_start[0] = L'\0';
	if (!pidgin_main) {
		DWORD dw = GetLastError();
		BOOL mod_not_found = (dw == ERROR_MOD_NOT_FOUND || dw == ERROR_DLL_NOT_FOUND);
		const wchar_t *err_msg = get_win32_error_message(dw);
		_snwprintf(errbuf, 512, L"Error loading pidgin.dll.\nError: (%u) %s%s%s",
			(UINT) dw, err_msg,
			mod_not_found ? L"\n" : L"",
			mod_not_found ? L"This probably means that GTK+ can't be found." : L"");
		wprintf(L"%s\n", errbuf);
		MessageBoxW(NULL, errbuf, L"Error", MB_OK | MB_TOPMOST);
		return 0;
	}
	/* Convert argv to utf-8*/
	szArglist = CommandLineToArgvW(cmdLine, &j);
	pidgin_argc = j;
	pidgin_argv = malloc(pidgin_argc* sizeof(char*));
	k = 0;
	for (i = 0; i < j; i++) {
		success = FALSE;
		/* Remove the --portable-mode arg from the args passed to pidgin so it doesn't choke */
		if (wcsstr(szArglist[i], L"--portable-mode") == NULL) {
			int len = WideCharToMultiByte(CP_UTF8, 0, szArglist[i],
				-1, NULL, 0, NULL, NULL);
			if (len != 0) {
				char *arg = malloc(len);
				len = WideCharToMultiByte(CP_UTF8, 0, szArglist[i],
					-1, arg, len, NULL, NULL);
				if (len != 0) {
					pidgin_argv[k++] = arg;
					success = TRUE;
				}
			}
			if (!success)
				wprintf(L"Error converting argument '%s' to UTF-8\n",
					szArglist[i]);
		}
		if (!success)
			pidgin_argc--;
	}
	LocalFree(szArglist);
	return pidgin_main(hInstance, pidgin_argc, pidgin_argv);
}
