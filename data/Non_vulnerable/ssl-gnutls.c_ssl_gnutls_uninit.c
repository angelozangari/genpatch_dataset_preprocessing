static void
ssl_gnutls_uninit(void)
{
	gnutls_global_deinit();
	gnutls_certificate_free_credentials(xcred);
	xcred = NULL;
#ifdef HAVE_GNUTLS_PRIORITY_FUNCS
	if (host_priorities) {
		g_hash_table_destroy(host_priorities);
		host_priorities = NULL;
	}
	gnutls_priority_deinit(default_priority);
	default_priority = NULL;
#endif
}
