 */
uin_t gg_tvbuff_read_uin(gg_tvbuff_t *tvb)
{
	uin_t uin = 0;
	uint32_t uin_len, full_len;
	uint8_t uin_type;
	const char *raw;
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	full_len = gg_tvbuff_read_packed_uint(tvb);
	uin_type = gg_tvbuff_read_uint8(tvb);
	uin_len = gg_tvbuff_read_uint8(tvb);
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	if (full_len != uin_len + 2 ||
		uin_type != 0 ||
		uin_len > 10)
	{
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_uin() failed (1)\n");
		tvb->valid = 0;
		return 0;
	}
	raw = gg_tvbuff_read_buff(tvb, uin_len);
	if (raw)
		uin = gg_str_to_uin(raw, uin_len);
	if (uin == 0) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_uin() failed (2)\n");
		tvb->valid = 0;
		return 0;
	}
	return uin;
}
