static PurpleCertificate *
x509_copy_certificate(PurpleCertificate *crt)
{
	CERTCertificate *crt_dat;
	PurpleCertificate *newcrt;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_nss, NULL);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, NULL);
	/* Create the certificate copy */
	newcrt = g_new0(PurpleCertificate, 1);
	newcrt->scheme = &x509_nss;
	/* NSS does refcounting automatically */
	newcrt->data = CERT_DupCertificate(crt_dat);
	return newcrt;
}
