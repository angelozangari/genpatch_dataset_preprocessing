static GByteArray *
x509_sha1sum(PurpleCertificate *crt)
{
	return x509_shasum(crt, GNUTLS_DIG_SHA1);
}
