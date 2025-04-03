static void
ssl_gnutls_log(int level, const char *str)
{
	/* GnuTLS log messages include the '\n' */
	purple_debug_misc("gnutls", "lvl %d: %s", level, str);
}
