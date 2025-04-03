static void
ssl_nss_init_nss(void)
{
#if NSS_VMAJOR > 3 || ( NSS_VMAJOR == 3 && NSS_VMINOR >= 14 )
	SSLVersionRange supported, enabled;
#endif /* NSS >= 3.14 */
	PR_Init(PR_SYSTEM_THREAD, PR_PRIORITY_NORMAL, 1);
	NSS_NoDB_Init(".");
#if (NSS_VMAJOR == 3 && (NSS_VMINOR < 15 || (NSS_VMINOR == 15 && NSS_VPATCH < 2)))
	NSS_SetDomesticPolicy();
#endif /* NSS < 3.15.2 */
	ssl_nss_init_ciphers();
#if NSS_VMAJOR > 3 || ( NSS_VMAJOR == 3 && NSS_VMINOR >= 14 )
	/* Get the ranges of supported and enabled SSL versions */
	if ((SSL_VersionRangeGetSupported(ssl_variant_stream, &supported) == SECSuccess) &&
			(SSL_VersionRangeGetDefault(ssl_variant_stream, &enabled) == SECSuccess)) {
		purple_debug_info("nss", "TLS supported versions: "
				"0x%04hx through 0x%04hx\n", supported.min, supported.max);
		purple_debug_info("nss", "TLS versions allowed by default: "
				"0x%04hx through 0x%04hx\n", enabled.min, enabled.max);
		/* Make sure all versions of TLS supported by the local library are
		   enabled. (For some reason NSS doesn't enable newer versions of TLS
		   by default -- more context in ticket #15909.) */
		if (supported.max > enabled.max) {
			enabled.max = supported.max;
			if (SSL_VersionRangeSetDefault(ssl_variant_stream, &enabled) == SECSuccess) {
				purple_debug_info("nss", "Changed allowed TLS versions to "
						"0x%04hx through 0x%04hx\n", enabled.min, enabled.max);
			} else {
				purple_debug_error("nss", "Error setting allowed TLS versions to "
						"0x%04hx through 0x%04hx\n", enabled.min, enabled.max);
			}
		}
	}
#endif /* NSS >= 3.14 */
	/** Disable OCSP Checking until we can make that use our HTTP & Proxy stuff */
	CERT_EnableOCSPChecking(PR_FALSE);
	_identity = PR_GetUniqueIdentity("Purple");
	_nss_methods = PR_GetDefaultIOMethods();
}
