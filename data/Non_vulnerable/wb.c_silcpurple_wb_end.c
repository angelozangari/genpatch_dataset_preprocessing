}
void silcpurple_wb_end(PurpleWhiteboard *wb)
{
	silc_free(wb->proto_data);
	wb->proto_data = NULL;
}
