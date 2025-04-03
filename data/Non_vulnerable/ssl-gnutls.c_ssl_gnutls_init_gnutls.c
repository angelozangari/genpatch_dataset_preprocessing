static void
ssl_gnutls_init_gnutls(void)
{
	const char *debug_level;
	const char *host_priorities_str;
	debug_level = g_getenv("PURPLE_GNUTLS_DEBUG");
	if (debug_level) {
		int level = atoi(debug_level);
		if (level < 0) {
			purple_debug_warning("gnutls", "Assuming log level 0 instead of %d\n",
			                     level);
			level = 0;
		}
		/* "The level is an integer between 0 and 9. Higher values mean more verbosity." */
		gnutls_global_set_log_level(level);
		gnutls_global_set_log_function(ssl_gnutls_log);
	}
	/* Expected format: host=priority;host2=priority;*=priority
	 * where "*" is used to override the default priority string for
	 * libpurple.
	 */
	host_priorities_str = g_getenv("PURPLE_GNUTLS_PRIORITIES");
	if (host_priorities_str) {
#ifndef HAVE_GNUTLS_PRIORITY_FUNCS
		purple_debug_warning("gnutls", "Warning, PURPLE_GNUTLS_PRIORITIES "
		                     "environment variable set, but we were built "
		                     "against an older GnuTLS that doesn't support "
		                     "this. :-(");
#else /* HAVE_GNUTLS_PRIORITY_FUNCS */
		char **entries = g_strsplit(host_priorities_str, ";", -1);
		char *default_priority_str = NULL;
		guint i;
		host_priorities = g_hash_table_new_full(g_str_hash, g_str_equal,
		                                        g_free, g_free);
		for (i = 0; entries[i]; ++i) {
			char *host = entries[i];
			char *equals = strchr(host, '=');
			char *prio_str;
			if (equals) {
				*equals = '\0';
				prio_str = equals + 1;
				/* Empty? */
				if (*prio_str == '\0') {
					purple_debug_warning("gnutls", "Ignoring empty priority "
					                               "string for %s\n", host);
				} else {
					/* TODO: Validate each of these and complain */
					if (g_str_equal(host, "*")) {
						/* Override the default priority */
						g_free(default_priority_str);
						default_priority_str = g_strdup(prio_str);
					} else
						g_hash_table_insert(host_priorities, g_strdup(host),
						                    g_strdup(prio_str));
				}
			}
		}
		if (default_priority_str) {
			if (gnutls_priority_init(&default_priority, default_priority_str, NULL)) {
				purple_debug_warning("gnutls", "Unable to set default priority to %s\n",
				                     default_priority_str);
				/* Versions of GnuTLS as of 2.8.6 (2010-03-31) don't free/NULL
				 * this on error.
				 */
				gnutls_free(default_priority);
				default_priority = NULL;
			}
			g_free(default_priority_str);
		}
		g_strfreev(entries);
#endif /* HAVE_GNUTLS_PRIORITY_FUNCS */
	}
#ifdef HAVE_GNUTLS_PRIORITY_FUNCS
	/* Make sure we set have a default priority! */
	if (!default_priority) {
		if (gnutls_priority_init(&default_priority, "NORMAL:%SSL3_RECORD_VERSION", NULL)) {
			/* See comment above about memory leak */
			gnutls_free(default_priority);
			gnutls_priority_init(&default_priority, "NORMAL", NULL);
		}
	}
#endif /* HAVE_GNUTLS_PRIORITY_FUNCS */
	gnutls_global_init();
	gnutls_certificate_allocate_credentials(&xcred);
	/* TODO: I can likely remove this */
	gnutls_certificate_set_x509_trust_file(xcred, "ca.pem",
		GNUTLS_X509_FMT_PEM);
}
