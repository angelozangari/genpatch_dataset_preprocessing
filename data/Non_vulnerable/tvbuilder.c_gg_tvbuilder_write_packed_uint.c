 */
void gg_tvbuilder_write_packed_uint(gg_tvbuilder_t *tvb, uint64_t value)
{
	uint8_t buff[9];
	uint64_t val_curr;
	int i, val_len = 0;
	if (!gg_tvbuilder_is_valid(tvb))
		return;
	val_curr = value;
	while (val_curr > 0) {
		val_curr >>= 7;
		val_len++;
	}
	if (val_len == 0)
		val_len = 1;
	if (val_len > 9) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuilder_write_packed_uint() "
			"int size too big (%d): %" PRIu64 "\n", val_len, value);
		tvb->valid = 0;
		return;
	}
	val_curr = value;
	for (i = 0; i < val_len; i++) {
		uint8_t raw = val_curr & 0x7F;
		val_curr >>= 7;
		if (i + 1 < val_len)
			raw |= 0x80;
		buff[i] = raw;
	}
	gg_tvbuilder_write_buff(tvb, (const char*)buff, val_len);
}
