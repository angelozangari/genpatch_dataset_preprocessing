void
purple_ssl_init(void)
{
	/* Although purple_ssl_is_supported will do the initialization on
	   command, SSL plugins tend to register CertificateSchemes as well
	   as providing SSL ops. */
	if (!ssl_init()) {
		purple_debug_error("sslconn", "Unable to initialize SSL.\n");
	}
}
