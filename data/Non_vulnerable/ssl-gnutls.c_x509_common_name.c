static gchar *
x509_common_name (PurpleCertificate *crt)
{
	gnutls_x509_crt_t cert_dat;
	gchar *cn = NULL;
	size_t cn_size;
	int ret;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_gnutls, NULL);
	cert_dat = X509_GET_GNUTLS_DATA(crt);
	/* Figure out the length of the Common Name */
	/* Claim that the buffer is size 0 so GnuTLS just tells us how much
	   space it needs */
	cn_size = 0;
	gnutls_x509_crt_get_dn_by_oid(cert_dat,
				      GNUTLS_OID_X520_COMMON_NAME,
				      0, /* First CN found, please */
				      0, /* Not in raw mode */
				      cn, &cn_size);
	/* Now allocate and get the Common Name */
	/* Old versions of GnuTLS have an off-by-one error in reporting
	   the size of the needed buffer in some functions, so allocate
	   an extra byte */
	cn = g_new0(gchar, ++cn_size);
	ret = gnutls_x509_crt_get_dn_by_oid(cert_dat,
					    GNUTLS_OID_X520_COMMON_NAME,
					    0, /* First CN found, please */
					    0, /* Not in raw mode */
					    cn, &cn_size);
	if (ret != 0) {
		purple_debug_error("gnutls/x509",
				   "Failed to get Common Name\n");
		g_free(cn);
		return NULL;
	}
	return cn;
}
