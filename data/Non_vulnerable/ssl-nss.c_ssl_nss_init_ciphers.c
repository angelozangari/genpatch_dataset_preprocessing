 * https://hg.mozilla.org/mozilla-central/log/default/security/manager/ssl/src/nsNSSComponent.cpp */
static void ssl_nss_init_ciphers(void) {
	/* Disable any ciphers that NSS might have enabled by default */
	const PRUint16 *cipher;
	for (cipher = SSL_GetImplementedCiphers(); *cipher != 0; ++cipher) {
		SSL_CipherPrefSetDefault(*cipher, PR_FALSE);
	}
	/* Now only set SSL/TLS ciphers we knew about at compile time */
	for (cipher = default_ciphers; *cipher != 0; ++cipher) {
		SSL_CipherPrefSetDefault(*cipher, PR_TRUE);
	}
	/* Now log the available and enabled Ciphers */
	for (cipher = SSL_GetImplementedCiphers(); *cipher != 0; ++cipher) {
		const PRUint16 suite = *cipher;
		SECStatus rv;
		PRBool enabled;
		SSLCipherSuiteInfo info;
		rv = SSL_CipherPrefGetDefault(suite, &enabled);
		if (rv != SECSuccess) {
			gchar *error_txt = get_error_text();
			purple_debug_warning("nss",
					"SSL_CipherPrefGetDefault didn't like value 0x%04x: %s\n",
					suite, error_txt);
			g_free(error_txt);
			continue;
		}
		rv = SSL_GetCipherSuiteInfo(suite, &info, (int)(sizeof info));
		if (rv != SECSuccess) {
			gchar *error_txt = get_error_text();
			purple_debug_warning("nss",
					"SSL_GetCipherSuiteInfo didn't like value 0x%04x: %s\n",
					suite, error_txt);
			g_free(error_txt);
			continue;
		}
		purple_debug_info("nss", "Cipher - %s: %s\n",
				info.cipherSuiteName,
				enabled ? "Enabled" : "Disabled");
	}
}
