static void
ssl_nss_uninit(void)
{
	NSS_Shutdown();
	PR_Cleanup();
	_nss_methods = NULL;
}
