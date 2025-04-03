 */
int aim_tlvlist_add_frozentlvlist(GSList **list, guint16 type, GSList **tlvlist)
{
	int buflen;
	ByteStream bs;
	buflen = aim_tlvlist_size(*tlvlist);
	if (buflen <= 0)
		return 0;
	byte_stream_new(&bs, buflen);
	aim_tlvlist_write(&bs, tlvlist);
	aim_tlvlist_add_raw(list, type, byte_stream_curpos(&bs), bs.data);
	byte_stream_destroy(&bs);
	return buflen;
}
