static PurpleCertificate *
x509_import_from_file(const gchar * filename)
{
	PurpleCertificate *crt;  /* Certificate being constructed */
	gchar *buf;        /* Used to load the raw file data */
	gsize buf_sz;      /* Size of the above */
	gnutls_datum_t dt; /* Struct to pass down to GnuTLS */
	purple_debug_info("gnutls",
			  "Attempting to load X.509 certificate from %s\n",
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
	/* Load the datum struct */
	dt.data = (unsigned char *) buf;
	dt.size = buf_sz;
	/* Perform the conversion; files should be in PEM format */
	crt = x509_import_from_datum(dt, GNUTLS_X509_FMT_PEM);
	/* Cleanup */
	g_free(buf);
	return crt;
}
