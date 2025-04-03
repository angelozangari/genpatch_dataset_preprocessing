static gboolean
plugin_load(PurplePlugin *plugin)
{
	if(!purple_ssl_get_ops()) {
		purple_ssl_set_ops(&ssl_ops);
	}
	/* Init GNUTLS now so others can use it even if sslconn never does */
	ssl_gnutls_init_gnutls();
	/* Register that we're providing an X.509 CertScheme */
	purple_certificate_register_scheme( &x509_gnutls );
	return TRUE;
}
