static gboolean
x509_compare_pubkeys (PurpleCertificate *crt1, PurpleCertificate *crt2)
{
	gnutls_x509_crt_t crt_dat1, crt_dat2;
	unsigned char buffer1[64], buffer2[64];
	size_t size1, size2;
	size1 = size2 = sizeof(buffer1);
	g_return_val_if_fail(crt1 && crt2, FALSE);
	g_return_val_if_fail(crt1->scheme == &x509_gnutls, FALSE);
	g_return_val_if_fail(crt2->scheme == &x509_gnutls, FALSE);
	crt_dat1 = X509_GET_GNUTLS_DATA(crt1);
	if (gnutls_x509_crt_get_key_id(crt_dat1, KEYID_FLAG, buffer1, &size1) != 0) {
		return FALSE;
	}
	crt_dat2 = X509_GET_GNUTLS_DATA(crt2);
	if (gnutls_x509_crt_get_key_id(crt_dat2, KEYID_FLAG, buffer2, &size2) != 0) {
		return FALSE;
	}
	if (size1 != size2) {
		return FALSE;
	}
	return memcmp(buffer1, buffer2, size1) == 0;
}
