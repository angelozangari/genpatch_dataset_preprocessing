static gboolean
x509_export_certificate(const gchar *filename, PurpleCertificate *crt)
{
	CERTCertificate *crt_dat;
	SECItem *dercrt;
	gchar *b64crt;
	gchar *pemcrt;
	gboolean ret = FALSE;
	g_return_val_if_fail(filename, FALSE);
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_nss, FALSE);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, FALSE);
	purple_debug_info("nss/x509",
			  "Exporting certificate to %s\n", filename);
	/* First, use NSS voodoo to create a DER-formatted certificate */
	dercrt = SEC_ASN1EncodeItem(NULL, NULL, crt_dat,
				    SEC_ASN1_GET(SEC_SignedCertificateTemplate));
	g_return_val_if_fail(dercrt != NULL, FALSE);
	/* Now encode it to b64 */
	b64crt = NSSBase64_EncodeItem(NULL, NULL, 0, dercrt);
	SECITEM_FreeItem(dercrt, PR_TRUE);
	g_return_val_if_fail(b64crt, FALSE);
	/* Wrap it in nice PEM header things */
	pemcrt = g_strdup_printf("-----BEGIN CERTIFICATE-----\n%s\n-----END CERTIFICATE-----\n", b64crt);
	PORT_Free(b64crt); /* Notice that b64crt was allocated by an NSS
			      function; hence, we'll let NSPR free it. */
	/* Finally, dump the silly thing to a file. */
	ret =  purple_util_write_data_to_file_absolute(filename, pemcrt, -1);
	g_free(pemcrt);
	return ret;
}
