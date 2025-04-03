static void
print_security_info(PRFileDesc *fd)
{
	SECStatus result;
	SSLChannelInfo channel;
	SSLCipherSuiteInfo suite;
	result = SSL_GetChannelInfo(fd, &channel, sizeof channel);
	if (result == SECSuccess && channel.length == sizeof channel
			&& channel.cipherSuite) {
		result = SSL_GetCipherSuiteInfo(channel.cipherSuite,
				&suite, sizeof suite);
		if (result == SECSuccess) {
			purple_debug_info("nss", "SSL version %d.%d using "
					"%d-bit %s with %d-bit %s MAC\n"
					"Server Auth: %d-bit %s, "
					"Key Exchange: %d-bit %s, "
					"Compression: %s\n"
					"Cipher Suite Name: %s\n",
					channel.protocolVersion >> 8,
					channel.protocolVersion & 0xff,
					suite.effectiveKeyBits,
					suite.symCipherName,
					suite.macBits,
					suite.macAlgorithmName,
					channel.authKeyBits,
					suite.authAlgorithmName,
					channel.keaKeyBits, suite.keaTypeName,
					channel.compressionMethodName,
					suite.cipherSuiteName);
		}
	}
}
