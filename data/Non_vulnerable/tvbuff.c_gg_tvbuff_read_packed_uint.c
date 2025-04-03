 */
uint64_t gg_tvbuff_read_packed_uint(gg_tvbuff_t *tvb)
{
	uint64_t val = 0;
	int i, val_len = 0;
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	while (gg_tvbuff_have_remaining(tvb, 1)) {
		val_len++;
		if (!(gg_tvbuff_read_uint8(tvb) & 0x80))
			break;
	}
	if (!gg_tvbuff_is_valid(tvb)) {
		gg_debug(GG_DEBUG_WARNING,
			"// gg_tvbuff_read_packed_uint() failed\n");
		return 0;
	}
	if (val_len > 9) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_packed_uint() "
			"packed uint size too big: %d\n", val_len);
		tvb->valid = 0;
		return 0;
	}
	for (i = 1; i <= val_len; i++) {
		uint64_t old_val = val;
		val <<= 7;
		if (old_val != (val >> 7)) {
			gg_debug(GG_DEBUG_WARNING,
				"// gg_tvbuff_read_packed_uint() overflow\n");
			tvb->valid = 0;
			return 0;
		}
		val |= (uint8_t)(tvb->buffer[tvb->offset - i] & ~0x80);
	}
	return val;
}
