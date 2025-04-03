static GByteArray *
x509_shasum(PurpleCertificate *crt, gnutls_digest_algorithm_t algo)
{
	size_t hashlen = (algo == GNUTLS_DIG_SHA1) ? 20 : 32;
	size_t tmpsz = hashlen; /* Throw-away variable for GnuTLS to stomp on*/
	gnutls_x509_crt_t crt_dat;
	GByteArray *hash; /**< Final hash container */
	guchar hashbuf[hashlen]; /**< Temporary buffer to contain hash */
	g_return_val_if_fail(crt, NULL);
	crt_dat = X509_GET_GNUTLS_DATA(crt);
	/* Extract the fingerprint */
	g_return_val_if_fail(
		0 == gnutls_x509_crt_get_fingerprint(crt_dat, algo,
						     hashbuf, &tmpsz),
		NULL);
	/* This shouldn't happen */
	g_return_val_if_fail(tmpsz == hashlen, NULL);
	/* Okay, now create and fill hash array */
	hash = g_byte_array_new();
	g_byte_array_append(hash, hashbuf, hashlen);
	return hash;
}
