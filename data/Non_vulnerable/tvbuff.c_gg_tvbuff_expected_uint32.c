 */
void gg_tvbuff_expected_uint32(gg_tvbuff_t *tvb, uint32_t value)
{
	uint32_t got;
	size_t offset;
	offset = tvb->offset;
	got = gg_tvbuff_read_uint32(tvb);
	if (!gg_tvbuff_is_valid(tvb))
		return;
	if (got != value)
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_expected_uint32() "
			"expected %#08x, but %#08x found at %" GG_SIZE_FMT "\n",
			value, got, offset);
}
