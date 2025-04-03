}
static void do_test1(PurpleSrvResponse *resp, int results, gpointer sdata) {
	const char *servername = sdata;
	int port = 3478;
	if(results) {
		servername = resp[0].hostname;
		port = resp[0].port;
	}
	purple_debug_info("stun", "got %d SRV responses, server: %s, port: %d\n",
		results, servername, port);
	purple_dnsquery_a_account(NULL, servername, port, hbn_cb, NULL);
	g_free(resp);
}
