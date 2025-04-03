/* Windows Cleanup */
void wpurple_cleanup(void) {
	purple_debug_info("wpurple", "wpurple_cleanup\n");
	/* winsock cleanup */
	WSACleanup();
	g_free(app_data_dir);
	g_free(install_dir);
	g_free(lib_dir);
	g_free(locale_dir);
	app_data_dir = NULL;
	install_dir = NULL;
	lib_dir = NULL;
	locale_dir = NULL;
	libpurpledll_hInstance = NULL;
}
