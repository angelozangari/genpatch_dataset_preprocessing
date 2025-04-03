static PurpleCertificate *
x509_import_from_nss(CERTCertificate* cert)
{
	/* New certificate to return */
	PurpleCertificate * crt;
	/* Allocate the certificate and load it with data */
	crt = g_new0(PurpleCertificate, 1);
	crt->scheme = &x509_nss;
	crt->data = CERT_DupCertificate(cert);
	return crt;
}
