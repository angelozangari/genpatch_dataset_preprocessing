static SECStatus
ssl_auth_cert(void *arg, PRFileDesc *socket, PRBool checksig, PRBool is_server)
{
	/* We just skip cert verification here, and will verify the whole chain
	 * in ssl_nss_handshake_cb, after the handshake is complete.
	 *
	 * The problem is, purple_certificate_verify is asynchronous and
	 * ssl_auth_cert should return the result synchronously (it may ask the
	 * user, if an unknown certificate should be trusted or not).
	 *
	 * Ideally, SSL_AuthCertificateHook/ssl_auth_cert should decide
	 * immediately, if the certificate chain is already trusted and possibly
	 * SSL_BadCertHook to deal with unknown certificates.
	 *
	 * Current implementation may not be ideal, but is no less secure in
	 * terms of MITM attack.
	 */
	return SECSuccess;
}
