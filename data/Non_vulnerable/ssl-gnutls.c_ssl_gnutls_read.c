static size_t
ssl_gnutls_read(PurpleSslConnection *gsc, void *data, size_t len)
{
	PurpleSslGnutlsData *gnutls_data = PURPLE_SSL_GNUTLS_DATA(gsc);
	ssize_t s;
	s = gnutls_record_recv(gnutls_data->session, data, len);
	if(s == GNUTLS_E_AGAIN || s == GNUTLS_E_INTERRUPTED) {
		s = -1;
		errno = EAGAIN;
#ifdef GNUTLS_E_PREMATURE_TERMINATION
	} else if (s == GNUTLS_E_PREMATURE_TERMINATION) {
		purple_debug_warning("gnutls", "Received a FIN on the TCP socket "
				"for %s. This either means that the remote server closed "
				"the socket without sending us a Close Notify alert or a "
				"man-in-the-middle injected a FIN into the TCP stream. "
				"Assuming it's the former.\n", gsc->host);
#else
	} else if (s == GNUTLS_E_UNEXPECTED_PACKET_LENGTH) {
		purple_debug_warning("gnutls", "Received packet of unexpected "
				"length on the TCP socket for %s. Among other "
				"possibilities this might mean that the remote server "
				"closed the socket without sending us a Close Notify alert. "
				"Assuming that's the case for compatibility, however, note "
				"that it's quite possible that we're incorrectly ignoing "
				"a real error.\n", gsc->host);
#endif
		/*
		 * Summary:
		 * Always treat a closed TCP connection as if the remote server cleanly
		 * terminated the SSL session.
		 *
		 * Background:
		 * Most TLS servers send a Close Notify alert before sending TCP FIN
		 * when closing a session. This informs us at the TLS layer that the
		 * connection is being cleanly closed. Without this it's more
		 * difficult for us to determine whether the session was closed
		 * cleanly (we would need to resort to having the application layer
		 * perform this check, e.g. by looking at the Content-Length HTTP
		 * header for HTTP connections).
		 *
		 * There ARE servers that don't send Close Notify and we want to be
		 * compatible with them. And so we don't require Close Notify. This
		 * seems to match the behavior of libnss. This is a slightly
		 * unfortunate situation. It means a malicious MITM can inject a FIN
		 * into our TCP stream and cause our encrypted session to termiate
		 * and we won't indicate any problem to the user.
		 *
		 * GnuTLS < 3.0.0 returned the UNEXPECTED_PACKET_LENGTH error on EOF.
		 * GnuTLS >= 3.0.0 added the PREMATURE_TERMINATION error to allow us
		 * to detect the problem more specifically.
		 *
		 * For historical discussion see:
		 * https://developer.pidgin.im/ticket/16172
		 * http://trac.adiumx.com/intertrac/ticket%3A16678
		 * https://bugzilla.mozilla.org/show_bug.cgi?id=508698#c4
		 * http://lists.gnu.org/archive/html/gnutls-devel/2008-03/msg00058.html
		 * Or search for GNUTLS_E_UNEXPECTED_PACKET_LENGTH or
		 * GNUTLS_E_PREMATURE_TERMINATION
		 */
		s = 0;
	} else if(s < 0) {
		purple_debug_error("gnutls", "receive failed: %s\n",
				gnutls_strerror(s));
		s = -1;
		/*
		 * TODO: Set errno to something more appropriate.  Or even
		 *       better: allow ssl plugins to keep track of their
		 *       own error message, then add a new ssl_ops function
		 *       that returns the error message.
		 */
		errno = EIO;
	}
	return s;
}
