}
static void x509_verify_cert(PurpleCertificateVerificationRequest *vrq, PurpleCertificateInvalidityFlags *flags)
{
	CERTCertDBHandle *certdb = CERT_GetDefaultCertDB();
	CERTCertificate *crt_dat;
	PRTime now = PR_Now();
	SECStatus rv;
	PurpleCertificate *first_cert = vrq->cert_chain->data;
	CERTVerifyLog log;
	gboolean self_signed = FALSE;
	crt_dat = X509_NSS_DATA(first_cert);
	log.arena = PORT_NewArena(512);
	log.head = log.tail = NULL;
	log.count = 0;
	rv = CERT_VerifyCert(certdb, crt_dat, PR_TRUE, certUsageSSLServer, now, NULL, &log);
	if (rv != SECSuccess || log.count > 0) {
		CERTVerifyLogNode *node   = NULL;
		unsigned int depth = (unsigned int)-1;
		if (crt_dat->isRoot) {
			self_signed = TRUE;
			*flags |= PURPLE_CERTIFICATE_SELF_SIGNED;
		}
		/* Handling of untrusted, etc. modeled after
		 * source/security/manager/ssl/src/TransportSecurityInfo.cpp in Firefox
		 */
		for (node = log.head; node; node = node->next) {
			if (depth != node->depth) {
				depth = node->depth;
				purple_debug_error("nss", "CERT %d. %s %s:\n", depth,
					node->cert->subjectName,
					depth ? "[Certificate Authority]": "");
			}
			purple_debug_error("nss", "  ERROR %ld: %s\n", node->error,
				PR_ErrorToName(node->error));
			switch (node->error) {
				case SEC_ERROR_EXPIRED_CERTIFICATE:
					*flags |= PURPLE_CERTIFICATE_EXPIRED;
					break;
				case SEC_ERROR_REVOKED_CERTIFICATE:
					*flags |= PURPLE_CERTIFICATE_REVOKED;
					break;
				case SEC_ERROR_UNKNOWN_ISSUER:
				case SEC_ERROR_UNTRUSTED_ISSUER:
					if (!self_signed) {
						*flags |= PURPLE_CERTIFICATE_CA_UNKNOWN;
					}
					break;
				case SEC_ERROR_CA_CERT_INVALID:
				case SEC_ERROR_EXPIRED_ISSUER_CERTIFICATE:
				case SEC_ERROR_UNTRUSTED_CERT:
#ifdef SEC_ERROR_CERT_SIGNATURE_ALGORITHM_DISABLED
				case SEC_ERROR_CERT_SIGNATURE_ALGORITHM_DISABLED:
#endif
					if (!self_signed) {
						*flags |= PURPLE_CERTIFICATE_INVALID_CHAIN;
					}
					break;
				case SEC_ERROR_BAD_SIGNATURE:
				default:
					*flags |= PURPLE_CERTIFICATE_INVALID_CHAIN;
			}
			if (node->cert)
				CERT_DestroyCertificate(node->cert);
		}
	}
	rv = CERT_VerifyCertName(crt_dat, vrq->subject_name);
	if (rv != SECSuccess) {
		purple_debug_error("nss", "subject name not verified\n");
		*flags |= PURPLE_CERTIFICATE_NAME_MISMATCH;
	}
	PORT_FreeArena(log.arena, PR_FALSE);
}
