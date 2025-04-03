static void
x509_destroy_certificate(PurpleCertificate * crt)
{
	CERTCertificate *crt_dat;
	g_return_if_fail(crt);
	g_return_if_fail(crt->scheme == &x509_nss);
	crt_dat = X509_NSS_DATA(crt);
	g_return_if_fail(crt_dat);
	/* Finally we have the certificate. So let's kill it */
	/* NSS does refcounting automatically */
	CERT_DestroyCertificate(crt_dat);
	/* Delete the PurpleCertificate as well */
	g_free(crt);
}
