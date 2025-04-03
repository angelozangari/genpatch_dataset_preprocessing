}
void wpurple_init(void) {
	WORD wVersionRequested;
	WSADATA wsaData;
	if (!g_thread_supported())
		g_thread_init(NULL);
	purple_debug_info("wpurple", "wpurple_init start\n");
	purple_debug_info("wpurple", "libpurple version: " DISPLAY_VERSION "\n");
	purple_debug_info("wpurple", "Glib:%u.%u.%u\n",
		glib_major_version, glib_minor_version, glib_micro_version);
	/* Winsock init */
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	/* Confirm that the winsock DLL supports 2.2 */
	/* Note that if the DLL supports versions greater than
	   2.2 in addition to 2.2, it will still return 2.2 in
	   wVersion since that is the version we requested. */
	if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		purple_debug_error("wpurple", "Could not find a usable WinSock DLL.  Oh well.\n");
		WSACleanup();
	}
	purple_debug_info("wpurple", "wpurple_init end\n");
}
