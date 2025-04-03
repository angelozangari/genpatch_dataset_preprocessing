static void
silcpurple_wb_request_cb(SilcPurpleWbRequest req, gint id)
{
	PurpleWhiteboard *wb;
        if (id != 1)
                goto out;
	if (!req->channel)
		wb = silcpurple_wb_init(req->sg, req->sender);
	else
		wb = silcpurple_wb_init_ch(req->sg, req->channel);
	silcpurple_wb_parse(wb->proto_data, wb, req->message, req->message_len);
  out:
	silc_free(req->message);
	silc_free(req);
}
