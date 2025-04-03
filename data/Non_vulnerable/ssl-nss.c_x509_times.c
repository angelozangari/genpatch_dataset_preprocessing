static gboolean
x509_times (PurpleCertificate *crt, time_t *activation, time_t *expiration)
{
	CERTCertificate *crt_dat;
	PRTime nss_activ, nss_expir;
	SECStatus cert_times_success;
	g_return_val_if_fail(crt, FALSE);
	g_return_val_if_fail(crt->scheme == &x509_nss, FALSE);
	crt_dat = X509_NSS_DATA(crt);
	g_return_val_if_fail(crt_dat, FALSE);
	/* Extract the times into ugly PRTime thingies */
	/* TODO: Maybe this shouldn't throw an error? */
	cert_times_success = CERT_GetCertTimes(crt_dat,
						&nss_activ, &nss_expir);
	g_return_val_if_fail(cert_times_success == SECSuccess, FALSE);
	/* NSS's native PRTime type *almost* corresponds to time_t; however,
	   it measures *microseconds* since the epoch, not seconds. Hence
	   the funny conversion. */
	nss_activ = nss_activ / 1000000;
	nss_expir = nss_expir / 1000000;
	if (activation) {
		*activation = nss_activ;
#if SIZEOF_TIME_T == 4
		/** Hack to deal with dates past the 32-bit barrier.
		    Handling is different for signed vs unsigned 32-bit types.
		 */
		if (*activation != nss_activ) {
			if (nss_activ < 0) {
				purple_debug_warning("nss",
					"Setting Activation Date to epoch to handle pre-epoch value\n");
				*activation = 0;
			} else {
				purple_debug_error("nss",
					"Activation date past 32-bit barrier, forcing invalidity\n");
				return FALSE;
			}
		}
#endif
	}
	if (expiration) {
		*expiration = nss_expir;
#if SIZEOF_TIME_T == 4
		if (*expiration != nss_expir) {
			if (*expiration < nss_expir) {
				if (*expiration < 0) {
					purple_debug_warning("nss",
						"Setting Expiration Date to 32-bit signed max\n");
					*expiration = PR_INT32_MAX;
				} else {
					purple_debug_warning("nss",
						"Setting Expiration Date to 32-bit unsigned max\n");
					*expiration = PR_UINT32_MAX;
				}
			} else {
				purple_debug_error("nss",
					"Expiration date prior to unix epoch, forcing invalidity\n");
				return FALSE;
			}
		}
#endif
	}
	return TRUE;
}
