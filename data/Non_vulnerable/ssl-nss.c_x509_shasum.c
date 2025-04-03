static GByteArray *
x509_shasum(PurpleCertificate *crt, SECOidTag algo)
{
	CERTCertificate *crt_dat;
	size_t hashlen = (algo == SEC_OID_SHA1) ? 20 : 32;
	GByteArray *hash;
	SECItem *derCert; /* DER representation of the cert */
	SECStatus st;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_nss, NULL);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, NULL);
	/* Get the certificate DER representation */
	derCert = &(crt_dat->derCert);
	/* Make a hash! */
	hash = g_byte_array_sized_new(hashlen);
	/* glib leaves the size as 0 by default */
	hash->len = hashlen;
	st = PK11_HashBuf(algo, hash->data,
			  derCert->data, derCert->len);
	/* Check for errors */
	if (st != SECSuccess) {
		g_byte_array_free(hash, TRUE);
		purple_debug_error("nss/x509",
				   "Error: hashing failed!\n");
		return NULL;
	}
	return hash;
}
