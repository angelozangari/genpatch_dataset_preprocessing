static x509_crtdata_t *
x509_crtdata_addref(x509_crtdata_t *cd)
{
	(cd->refcount)++;
	return cd;
}
