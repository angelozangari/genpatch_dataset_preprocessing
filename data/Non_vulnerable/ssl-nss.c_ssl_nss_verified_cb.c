static void
ssl_nss_verified_cb(PurpleCertificateVerificationStatus st,
		       gpointer userdata)
{
	PurpleSslConnection *gsc = (PurpleSslConnection *) userdata;
	if (st == PURPLE_CERTIFICATE_VALID) {
		/* Certificate valid? Good! Do the connection! */
		gsc->connect_cb(gsc->connect_cb_data, gsc, PURPLE_INPUT_READ);
	} else {
		/* Otherwise, signal an error */
		if(gsc->error_cb != NULL)
			gsc->error_cb(gsc, PURPLE_SSL_CERTIFICATE_INVALID,
				      gsc->connect_cb_data);
		purple_ssl_close(gsc);
	}
}
