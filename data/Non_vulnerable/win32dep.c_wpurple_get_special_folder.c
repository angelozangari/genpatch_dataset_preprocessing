/* Get paths to special Windows folders. */
gchar *wpurple_get_special_folder(int folder_type) {
	gchar *retval = NULL;
	wchar_t utf_16_dir[MAX_PATH + 1];
	if (SUCCEEDED(SHGetFolderPathW(NULL, folder_type, NULL,
					SHGFP_TYPE_CURRENT, utf_16_dir))) {
		retval = g_utf16_to_utf8(utf_16_dir, -1, NULL, NULL, NULL);
	}
	return retval;
}
