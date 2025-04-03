static gboolean
plugin_load(PurplePlugin *plugin)
{
	if (!purple_ssl_get_ops()) {
		purple_ssl_set_ops(&ssl_ops);
	}
	/* Init NSS now, so others can use it even if sslconn never does */
	ssl_nss_init_nss();
	/* Register the X.509 functions we provide */
	purple_certificate_register_scheme(&x509_nss);
	return TRUE;
}
