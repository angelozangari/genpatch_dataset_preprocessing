static gboolean
x509_certificate_signed_by(PurpleCertificate * crt,
			   PurpleCertificate * issuer)
{
	gnutls_x509_crt_t crt_dat;
	gnutls_x509_crt_t issuer_dat;
	unsigned int verify; /* used to store result from GnuTLS verifier */
	int ret;
	gchar *crt_id = NULL;
	gchar *issuer_id = NULL;
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(issuer, FALSE);
	/* Verify that both certs are the correct scheme */
	g_return_val_if_fail(crt->scheme == &x509_gnutls, FALSE);
	g_return_val_if_fail(issuer->scheme == &x509_gnutls, FALSE);
	/* TODO: check for more nullness? */
	crt_dat = X509_GET_GNUTLS_DATA(crt);
	issuer_dat = X509_GET_GNUTLS_DATA(issuer);
	/* Ensure crt issuer matches the name on the issuer cert. */
	ret = gnutls_x509_crt_check_issuer(crt_dat, issuer_dat);
	if (ret <= 0) {
		if (ret < 0) {
			purple_debug_error("gnutls/x509",
					   "GnuTLS error %d while checking certificate issuer match.",
					   ret);
		} else {
			gchar *crt_id, *issuer_id, *crt_issuer_id;
			crt_id = purple_certificate_get_unique_id(crt);
			issuer_id = purple_certificate_get_unique_id(issuer);
			crt_issuer_id =
				purple_certificate_get_issuer_unique_id(crt);
			purple_debug_info("gnutls/x509",
					  "Certificate %s is issued by "
					  "%s, which does not match %s.\n",
					  crt_id ? crt_id : "(null)",
					  crt_issuer_id ? crt_issuer_id : "(null)",
					  issuer_id ? issuer_id : "(null)");
			g_free(crt_id);
			g_free(issuer_id);
			g_free(crt_issuer_id);
		}
		/* The issuer is not correct, or there were errors */
		return FALSE;
	}
	/* Check basic constraints extension (if it exists then the CA flag must
	   be set to true, and it must exist for certs with version 3 or higher. */
	ret = gnutls_x509_crt_get_basic_constraints(issuer_dat, NULL, NULL, NULL);
	if (ret == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
		if (gnutls_x509_crt_get_version(issuer_dat) >= 3) {
			/* Reject cert (no basic constraints and cert version is >= 3). */
			gchar *issuer_id = purple_certificate_get_unique_id(issuer);
			purple_debug_info("gnutls/x509", "Rejecting cert because the "
					"basic constraints extension is missing from issuer cert "
					"for %s. The basic constraints extension is required on "
					"all version 3 or higher certs (this cert is version %d).",
					issuer_id ? issuer_id : "(null)",
					gnutls_x509_crt_get_version(issuer_dat));
			g_free(issuer_id);
			return FALSE;
		} else {
			/* Allow cert (no basic constraints and cert version is < 3). */
			purple_debug_info("gnutls/x509", "Basic constraint extension is "
					"missing from issuer cert for %s. Allowing this because "
					"the cert is version %d and the basic constraints "
					"extension is only required for version 3 or higher "
					"certs.", issuer_id ? issuer_id : "(null)",
					gnutls_x509_crt_get_version(issuer_dat));
		}
	} else if (ret <= 0) {
		/* Reject cert (CA flag is false in basic constraints). */
		gchar *issuer_id = purple_certificate_get_unique_id(issuer);
		purple_debug_info("gnutls/x509", "Rejecting cert because the CA flag "
				"is set to false in the basic constraints extension for "
				"issuer cert %s. ret=%d\n",
				issuer_id ? issuer_id : "(null)", ret);
		g_free(issuer_id);
		return FALSE;
	}
	/* Now, check the signature */
	/* The second argument is a ptr to an array of "trusted" issuer certs,
	   but we're only using one trusted one */
	ret = gnutls_x509_crt_verify(crt_dat, &issuer_dat, 1,
				     /* Permit signings by X.509v1 certs
					(Verisign and possibly others have
					root certificates that predate the
					current standard) */
				     GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT,
				     &verify);
	if (ret != 0) {
		purple_debug_error("gnutls/x509",
				   "Attempted certificate verification caused a GnuTLS error code %d. I will just say the signature is bad, but you should look into this.\n", ret);
		return FALSE;
	}
#ifdef HAVE_GNUTLS_CERT_INSECURE_ALGORITHM
	if (verify & GNUTLS_CERT_INSECURE_ALGORITHM) {
		/*
		 * A certificate in the chain is signed with an insecure
		 * algorithm. Put a warning into the log to make this error
		 * perfectly clear as soon as someone looks at the debug log is
		 * generated.
		 */
		crt_id = purple_certificate_get_unique_id(crt);
		issuer_id = purple_certificate_get_issuer_unique_id(crt);
		purple_debug_warning("gnutls/x509",
				"Insecure hash algorithm used by %s to sign %s\n",
				issuer_id, crt_id);
	}
#endif
	if (verify & GNUTLS_CERT_INVALID) {
		/* Signature didn't check out, but at least
		   there were no errors*/
		if (!crt_id)
			crt_id = purple_certificate_get_unique_id(crt);
		if (!issuer_id)
			issuer_id = purple_certificate_get_issuer_unique_id(crt);
		purple_debug_error("gnutls/x509",
				  "Bad signature from %s on %s\n",
				  issuer_id, crt_id);
		g_free(crt_id);
		g_free(issuer_id);
		return FALSE;
	} /* if (ret, etc.) */
	/* If we got here, the signature is good */
	return TRUE;
}
