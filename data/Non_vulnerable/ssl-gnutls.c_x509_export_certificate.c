static gboolean
x509_export_certificate(const gchar *filename, PurpleCertificate *crt)
{
	gnutls_x509_crt_t crt_dat; /* GnuTLS cert struct */
	int ret;
	gchar * out_buf; /* Data to output */
	size_t out_size; /* Output size */
	gboolean success = FALSE;
	/* Paranoia paranoia paranoia! */
	g_return_val_if_fail(filename, FALSE);
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_gnutls, FALSE);
	g_return_val_if_fail(crt->data, FALSE);
	crt_dat = X509_GET_GNUTLS_DATA(crt);
	/* Obtain the output size required */
	out_size = 0;
	ret = gnutls_x509_crt_export(crt_dat, GNUTLS_X509_FMT_PEM,
				     NULL, /* Provide no buffer yet */
				     &out_size /* Put size here */
		);
	g_return_val_if_fail(ret == GNUTLS_E_SHORT_MEMORY_BUFFER, FALSE);
	/* Now allocate a buffer and *really* export it */
	out_buf = g_new0(gchar, out_size);
	ret = gnutls_x509_crt_export(crt_dat, GNUTLS_X509_FMT_PEM,
				     out_buf, /* Export to our new buffer */
				     &out_size /* Put size here */
		);
	if (ret != 0) {
		purple_debug_error("gnutls/x509",
				   "Failed to export cert to buffer with code %d\n",
				   ret);
		g_free(out_buf);
		return FALSE;
	}
	/* Write it out to an actual file */
	success = purple_util_write_data_to_file_absolute(filename,
							  out_buf, out_size);
	g_free(out_buf);
	return success;
}
