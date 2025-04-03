static void
recv_cb(gpointer data, gint source, PurpleInputCondition cond)
{
	PurpleSslConnection *gsc = data;
	gsc->recv_cb(gsc->recv_cb_data, gsc, cond);
}
