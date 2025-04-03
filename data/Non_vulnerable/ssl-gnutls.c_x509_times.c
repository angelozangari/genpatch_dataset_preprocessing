static gboolean
x509_times (PurpleCertificate *crt, time_t *activation, time_t *expiration)
{
	gnutls_x509_crt_t crt_dat;
	/* GnuTLS time functions return this on error */
	const time_t errval = (time_t) (-1);
	gboolean success = TRUE;
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_gnutls, FALSE);
	crt_dat = X509_GET_GNUTLS_DATA(crt);
	if (activation) {
		*activation = gnutls_x509_crt_get_activation_time(crt_dat);
		if (*activation == errval)
			success = FALSE;
	}
	if (expiration) {
		*expiration = gnutls_x509_crt_get_expiration_time(crt_dat);
		if (*expiration == errval)
			success = FALSE;
	}
	return success;
}
