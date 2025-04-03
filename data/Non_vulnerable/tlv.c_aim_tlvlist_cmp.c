 */
int aim_tlvlist_cmp(GSList *one, GSList *two)
{
	ByteStream bs1, bs2;
	if (aim_tlvlist_size(one) != aim_tlvlist_size(two))
		return 1;
	byte_stream_new(&bs1, aim_tlvlist_size(one));
	byte_stream_new(&bs2, aim_tlvlist_size(two));
	aim_tlvlist_write(&bs1, &one);
	aim_tlvlist_write(&bs2, &two);
	if (memcmp(bs1.data, bs2.data, bs1.len)) {
		byte_stream_destroy(&bs1);
		byte_stream_destroy(&bs2);
		return 1;
	}
	byte_stream_destroy(&bs1);
	byte_stream_destroy(&bs2);
	return 0;
}
