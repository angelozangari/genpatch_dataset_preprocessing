static gchar *
x509_issuer_dn (PurpleCertificate *crt)
{
	gnutls_x509_crt_t cert_dat;
	gchar *dn = NULL;
	size_t dn_size;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_gnutls, NULL);
	cert_dat = X509_GET_GNUTLS_DATA(crt);
	/* Figure out the length of the Distinguished Name */
	/* Claim that the buffer is size 0 so GnuTLS just tells us how much
	   space it needs */
	dn_size = 0;
	gnutls_x509_crt_get_issuer_dn(cert_dat, dn, &dn_size);
	/* Now allocate and get the Distinguished Name */
	/* Old versions of GnuTLS have an off-by-one error in reporting
	   the size of the needed buffer in some functions, so allocate
	   an extra byte */
	dn = g_new0(gchar, ++dn_size);
	if (0 != gnutls_x509_crt_get_issuer_dn(cert_dat, dn, &dn_size)) {
		purple_debug_error("gnutls/x509",
				   "Failed to get issuer's Distinguished "
				   "Name\n");
		g_free(dn);
		return NULL;
	}
	return dn;
}
