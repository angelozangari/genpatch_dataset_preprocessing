static gboolean
x509_register_trusted_tls_cert(PurpleCertificate *crt, gboolean ca)
{
	CERTCertDBHandle *certdb = CERT_GetDefaultCertDB();
	CERTCertificate *crt_dat;
	CERTCertTrust trust;
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_nss, FALSE);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, FALSE);
	purple_debug_info("nss", "Trusting %s\n", crt_dat->subjectName);
	if (ca && !CERT_IsCACert(crt_dat, NULL)) {
		purple_debug_error("nss",
			"Refusing to set non-CA cert as trusted CA\n");
		return FALSE;
	}
	if (crt_dat->isperm) {
		purple_debug_info("nss",
			"Skipping setting trust for cert in permanent DB\n");
		return TRUE;
	}
	if (ca) {
		trust.sslFlags = CERTDB_TRUSTED_CA | CERTDB_TRUSTED_CLIENT_CA;
	} else {
		trust.sslFlags = CERTDB_TRUSTED;
	}
	trust.emailFlags = 0;
	trust.objectSigningFlags = 0;
	CERT_ChangeCertTrust(certdb, crt_dat, &trust);
	return TRUE;
}
