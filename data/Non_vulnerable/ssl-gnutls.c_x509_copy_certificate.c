static PurpleCertificate *
x509_copy_certificate(PurpleCertificate *crt)
{
	x509_crtdata_t *crtdat;
	PurpleCertificate *newcrt;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_gnutls, NULL);
	crtdat = (x509_crtdata_t *) crt->data;
	newcrt = g_new0(PurpleCertificate, 1);
	newcrt->scheme = &x509_gnutls;
	newcrt->data = x509_crtdata_addref(crtdat);
	return newcrt;
}
