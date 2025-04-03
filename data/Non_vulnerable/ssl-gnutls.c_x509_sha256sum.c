static GByteArray *
x509_sha256sum(PurpleCertificate *crt)
{
	return x509_shasum(crt, GNUTLS_DIG_SHA256);
}
