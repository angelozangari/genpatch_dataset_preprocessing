}
void *jabber_x_data_request(JabberStream *js, xmlnode *packet, jabber_x_data_cb cb, gpointer user_data)
{
	return jabber_x_data_request_with_actions(js, packet, NULL, 0, (jabber_x_data_action_cb)cb, user_data);
}
