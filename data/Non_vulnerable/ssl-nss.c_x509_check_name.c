static gboolean
x509_check_name (PurpleCertificate *crt, const gchar *name)
{
	CERTCertificate *crt_dat;
	SECStatus st;
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_nss, FALSE);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, FALSE);
	st = CERT_VerifyCertName(crt_dat, name);
	if (st == SECSuccess) {
		return TRUE;
	}
	else if (st == SECFailure) {
		return FALSE;
	}
	/* If we get here...bad things! */
	purple_debug_error("nss/x509",
			   "x509_check_name fell through where it shouldn't "
			   "have.\n");
	return FALSE;
}
