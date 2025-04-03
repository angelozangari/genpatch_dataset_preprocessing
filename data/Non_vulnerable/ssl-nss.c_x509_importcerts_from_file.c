static GSList *
x509_importcerts_from_file(const gchar *filename)
{
	gchar *rawcert, *begin, *end;
	gsize len = 0;
	GSList *crts = NULL;
	CERTCertificate *crt_dat;
	PurpleCertificate *crt;
	g_return_val_if_fail(filename != NULL, NULL);
	purple_debug_info("nss/x509",
			  "Loading certificate from %s\n",
			  filename);
	/* Load the raw data up */
	if (!g_file_get_contents(filename,
				 &rawcert, &len,
				 NULL)) {
		purple_debug_error("nss/x509", "Unable to read certificate file.\n");
		return NULL;
	}
	if (len == 0) {
		purple_debug_error("nss/x509",
				"Certificate file has no contents!\n");
		if (rawcert)
			g_free(rawcert);
		return NULL;
	}
	begin = rawcert;
	while((end = strstr(begin, "-----END CERTIFICATE-----")) != NULL) {
		end += sizeof("-----END CERTIFICATE-----")-1;
		/* Decode the certificate */
		crt_dat = CERT_DecodeCertFromPackage(begin, (end-begin));
		g_return_val_if_fail(crt_dat != NULL, NULL);
		crt = g_new0(PurpleCertificate, 1);
		crt->scheme = &x509_nss;
		crt->data = crt_dat;
		crts = g_slist_prepend(crts, crt);
		begin = end;
	}
	g_free(rawcert);
	return crts;
}
