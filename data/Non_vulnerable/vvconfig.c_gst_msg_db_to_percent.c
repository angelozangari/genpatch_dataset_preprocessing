static gdouble
gst_msg_db_to_percent(GstMessage *msg, gchar *value_name)
{
	const GValue *list;
	const GValue *value;
	gdouble value_db;
	gdouble percent;
	list = gst_structure_get_value(
				gst_message_get_structure(msg), value_name);
#if GST_CHECK_VERSION(1,0,0)
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	value = g_value_array_get_nth(g_value_get_boxed(list), 0);
G_GNUC_END_IGNORE_DEPRECATIONS
#else
	value = gst_value_list_get_value(list, 0);
#endif
	value_db = g_value_get_double(value);
	percent = pow(10, value_db / 20);
	return (percent > 1.0) ? 1.0 : percent;
}
