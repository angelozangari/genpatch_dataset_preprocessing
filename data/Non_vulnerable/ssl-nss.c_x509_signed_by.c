static gboolean
x509_signed_by(PurpleCertificate * crt,
	       PurpleCertificate * issuer)
{
	CERTCertificate *subjectCert;
	CERTCertificate *issuerCert;
	SECStatus st;
	issuerCert = X509_NSS_DATA(issuer);
	g_return_val_if_fail(issuerCert, FALSE);
	subjectCert = X509_NSS_DATA(crt);
	g_return_val_if_fail(subjectCert, FALSE);
	if (subjectCert->issuerName == NULL || issuerCert->subjectName == NULL
			|| PORT_Strcmp(subjectCert->issuerName, issuerCert->subjectName) != 0)
		return FALSE;
	st = CERT_VerifySignedData(&subjectCert->signatureWrap, issuerCert, PR_Now(), NULL);
	return st == SECSuccess;
}
