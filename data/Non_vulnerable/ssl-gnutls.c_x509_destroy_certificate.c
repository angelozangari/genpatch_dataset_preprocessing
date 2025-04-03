static void
x509_destroy_certificate(PurpleCertificate * crt)
{
	if (NULL == crt) return;
	/* Check that the scheme is x509_gnutls */
	if ( crt->scheme != &x509_gnutls ) {
		purple_debug_error("gnutls",
				   "destroy_certificate attempted on certificate of wrong scheme (scheme was %s, expected %s)\n",
				   crt->scheme->name,
				   SCHEME_NAME);
		return;
	}
	g_return_if_fail(crt->data != NULL);
	g_return_if_fail(crt->scheme != NULL);
	/* Use the reference counting system to free (or not) the
	   underlying data */
	x509_crtdata_delref((x509_crtdata_t *)crt->data);
	/* Kill the structure itself */
	g_free(crt);
}
