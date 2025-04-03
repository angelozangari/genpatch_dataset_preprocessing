const gchar *
purple_ssl_strerror(PurpleSslErrorType error)
{
	switch(error) {
		case PURPLE_SSL_CONNECT_FAILED:
			return _("SSL Connection Failed");
		case PURPLE_SSL_HANDSHAKE_FAILED:
			return _("SSL Handshake Failed");
		case PURPLE_SSL_CERTIFICATE_INVALID:
			return _("SSL peer presented an invalid certificate");
		default:
			purple_debug_warning("sslconn", "Unknown SSL error code %d\n", error);
			return _("Unknown SSL error");
	}
}
