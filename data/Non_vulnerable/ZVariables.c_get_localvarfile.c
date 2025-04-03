}
static char *get_localvarfile(void)
{
    const char *base;
#ifndef WIN32
    struct passwd *pwd;
    base = purple_home_dir();
#else
    base = getenv("HOME");
    if (!base)
        base = getenv("HOMEPATH");
    if (!base)
        base = "C:\\";
#endif
    if (!base) {
#ifndef WIN32
	if (!(pwd = getpwuid((int) getuid()))) {
	    fprintf(stderr, "Zephyr internal failure: Can't find your entry in /etc/passwd\n");
	    return NULL;
	}
	base = pwd->pw_dir;
#endif
    }
    return g_strconcat(base, "/.zephyr.vars", NULL);
}
