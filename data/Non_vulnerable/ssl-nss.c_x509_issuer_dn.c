static gchar *
x509_issuer_dn (PurpleCertificate *crt)
{
	CERTCertificate *crt_dat;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_nss, NULL);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, NULL);
	return g_strdup(crt_dat->issuerName);
}
