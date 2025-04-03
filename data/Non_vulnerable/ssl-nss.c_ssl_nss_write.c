static size_t
ssl_nss_write(PurpleSslConnection *gsc, const void *data, size_t len)
{
	PRInt32 ret;
	PurpleSslNssData *nss_data = PURPLE_SSL_NSS_DATA(gsc);
	if(!nss_data)
		return 0;
	ret = PR_Write(nss_data->in, data, len);
	if (ret == -1)
		set_errno(PR_GetError());
	return ret;
}
