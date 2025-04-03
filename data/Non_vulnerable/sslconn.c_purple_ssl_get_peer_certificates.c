GList *
purple_ssl_get_peer_certificates(PurpleSslConnection *gsc)
{
	PurpleSslOps *ops;
	g_return_val_if_fail(gsc != NULL, NULL);
	ops = purple_ssl_get_ops();
	return (ops->get_peer_certificates)(gsc);
}
