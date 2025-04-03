static size_t
ssl_nss_read(PurpleSslConnection *gsc, void *data, size_t len)
{
	PRInt32 ret;
	PurpleSslNssData *nss_data = PURPLE_SSL_NSS_DATA(gsc);
	ret = PR_Read(nss_data->in, data, len);
	if (ret == -1)
		set_errno(PR_GetError());
	return ret;
}
