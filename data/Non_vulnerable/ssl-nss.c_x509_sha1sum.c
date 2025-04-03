static GByteArray *
x509_sha1sum(PurpleCertificate *crt)
{
	return x509_shasum(crt, SEC_OID_SHA1);
}
