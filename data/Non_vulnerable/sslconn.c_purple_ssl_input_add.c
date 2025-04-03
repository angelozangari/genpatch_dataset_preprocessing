void
purple_ssl_input_add(PurpleSslConnection *gsc, PurpleSslInputFunction func,
				   void *data)
{
	g_return_if_fail(func != NULL);
	g_return_if_fail(purple_ssl_is_supported());
	gsc->recv_cb_data = data;
	gsc->recv_cb      = func;
	gsc->inpa = purple_input_add(gsc->fd, PURPLE_INPUT_READ, recv_cb, gsc);
}
