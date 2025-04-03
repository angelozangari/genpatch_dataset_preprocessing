static GSList *
x509_importcerts_from_file(const gchar * filename)
{
	PurpleCertificate *crt;  /* Certificate being constructed */
	gchar *buf;        /* Used to load the raw file data */
	gchar *begin, *end;
	GSList *crts = NULL;
	gsize buf_sz;      /* Size of the above */
	gnutls_datum_t dt; /* Struct to pass down to GnuTLS */
	purple_debug_info("gnutls",
			  "Attempting to load X.509 certificates from %s\n",
			  filename);
	/* Next, we'll simply yank the entire contents of the file
	   into memory */
	/* TODO: Should I worry about very large files here? */
	g_return_val_if_fail(
		g_file_get_contents(filename,
			    &buf,
			    &buf_sz,
			    NULL      /* No error checking for now */
		),
		NULL);
	begin = buf;
	while((end = strstr(begin, "-----END CERTIFICATE-----")) != NULL) {
		end += sizeof("-----END CERTIFICATE-----")-1;
		/* Load the datum struct */
		dt.data = (unsigned char *) begin;
		dt.size = (end-begin);
		/* Perform the conversion; files should be in PEM format */
		crt = x509_import_from_datum(dt, GNUTLS_X509_FMT_PEM);
		if (crt != NULL) {
			crts = g_slist_prepend(crts, crt);
		}
		begin = end;
	}
	/* Cleanup */
	g_free(buf);
	return crts;
}
