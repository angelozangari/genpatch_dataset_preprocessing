*/
char *wpurple_escape_dirsep(const char *filename) {
	int sepcount = 0;
	const char *tmp = filename;
	char *ret;
	int cnt = 0;
	g_return_val_if_fail(filename != NULL, NULL);
	while(*tmp) {
		if(*tmp == '\\')
			sepcount++;
		tmp++;
	}
	ret = g_malloc0(strlen(filename) + sepcount + 1);
	while(*filename) {
		ret[cnt] = *filename;
		if(*filename == '\\')
			ret[++cnt] = '\\';
		filename++;
		cnt++;
	}
	ret[cnt] = '\0';
	return ret;
}
