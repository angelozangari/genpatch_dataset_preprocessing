static gboolean
plugin_unload(PurplePlugin *plugin)
{
	if(purple_ssl_get_ops() == &ssl_ops) {
		purple_ssl_set_ops(NULL);
	}
	purple_certificate_unregister_scheme( &x509_gnutls );
	return TRUE;
}
