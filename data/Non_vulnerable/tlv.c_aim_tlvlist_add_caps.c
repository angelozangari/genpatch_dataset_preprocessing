 */
int aim_tlvlist_add_caps(GSList **list, const guint16 type, const guint64 caps, const char *mood)
{
	int len;
	ByteStream bs;
	guint32 bs_size;
	guint8 *data;
	if (caps == 0)
		return 0; /* nothing there anyway */
	data = icq_get_custom_icon_data(mood);
	bs_size = 16*(count_caps(caps) + (data != NULL ? 1 : 0));
	byte_stream_new(&bs, bs_size);
	byte_stream_putcaps(&bs, caps);
	/* adding of custom icon GUID */
	if (data != NULL)
		byte_stream_putraw(&bs, data, 16);
	len = aim_tlvlist_add_raw(list, type, byte_stream_curpos(&bs), bs.data);
	byte_stream_destroy(&bs);
	return len;
}
