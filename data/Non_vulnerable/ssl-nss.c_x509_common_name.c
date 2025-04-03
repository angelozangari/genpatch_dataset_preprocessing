static gchar *
x509_common_name (PurpleCertificate *crt)
{
	CERTCertificate *crt_dat;
	char *nss_cn;
	gchar *ret_cn;
	g_return_val_if_fail(crt, NULL);
	g_return_val_if_fail(crt->scheme == &x509_nss, NULL);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, NULL);
	/* Q:
	   Why get a newly allocated string out of NSS, strdup it, and then
	   return the new copy?
	   A:
	   The NSS LXR docs state that I should use the NSPR free functions on
	   the strings that the NSS cert functions return. Since the libpurple
	   API expects a g_free()-able string, we make our own copy and return
	   that.
	   NSPR is something of a prima donna. */
	nss_cn = CERT_GetCommonName( &(crt_dat->subject) );
	ret_cn = g_strdup(nss_cn);
	PORT_Free(nss_cn);
	return ret_cn;
}
