static gboolean
x509_check_name (PurpleCertificate *crt, const gchar *name)
{
	gnutls_x509_crt_t crt_dat;
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_gnutls, FALSE);
	g_return_val_if_fail(name, FALSE);
	crt_dat = X509_GET_GNUTLS_DATA(crt);
	if (gnutls_x509_crt_check_hostname(crt_dat, name)) {
		return TRUE;
	} else {
		return FALSE;
	}
}
