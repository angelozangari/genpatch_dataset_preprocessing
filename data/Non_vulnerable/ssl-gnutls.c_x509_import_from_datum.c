static PurpleCertificate *
x509_import_from_datum(const gnutls_datum_t dt, gnutls_x509_crt_fmt_t mode)
{
	/* Internal certificate data structure */
	x509_crtdata_t *certdat;
	/* New certificate to return */
	PurpleCertificate * crt;
	/* Allocate and prepare the internal certificate data */
	certdat = g_new0(x509_crtdata_t, 1);
	if (gnutls_x509_crt_init(&(certdat->crt)) != 0) {
		g_free(certdat);
		return NULL;
	}
	certdat->refcount = 0;
	/* Perform the actual certificate parse */
	/* Yes, certdat->crt should be passed as-is */
	if (gnutls_x509_crt_import(certdat->crt, &dt, mode) != 0) {
		g_free(certdat);
		return NULL;
	}
	/* Allocate the certificate and load it with data */
	crt = g_new0(PurpleCertificate, 1);
	crt->scheme = &x509_gnutls;
	crt->data = x509_crtdata_addref(certdat);
	return crt;
}
