static void
x509_crtdata_delref(x509_crtdata_t *cd)
{
	(cd->refcount)--;
	if (cd->refcount < 0)
		g_critical("Refcount of x509_crtdata_t is %d, which is less "
				"than zero!\n", cd->refcount);
	/* If the refcount reaches zero, kill the structure */
	if (cd->refcount <= 0) {
		/* Kill the internal data */
		gnutls_x509_crt_deinit( cd->crt );
		/* And kill the struct */
		g_free( cd );
	}
}
