static GByteArray *
x509_sha256sum(PurpleCertificate *crt)
{
	return x509_shasum(crt, SEC_OID_SHA256);
}
