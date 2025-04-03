 */
void gg_tvbuff_expected_uint8(gg_tvbuff_t *tvb, uint8_t value)
{
	uint8_t got;
	size_t offset;
	offset = tvb->offset;
	got = gg_tvbuff_read_uint8(tvb);
	if (!gg_tvbuff_is_valid(tvb))
		return;
	if (got != value)
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_expected_uint8() "
			"expected %#02x, but %#02x found at %" GG_SIZE_FMT "\n",
			value, got, offset);
}
