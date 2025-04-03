 */
uint64_t gg_tvbuff_read_uint64(gg_tvbuff_t *tvb)
{
	uint64_t val;
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	if (!gg_tvbuff_have_remaining(tvb, 8)) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_uint64() "
			"failed at %" GG_SIZE_FMT "\n", tvb->offset);
		return 0;
	}
	memcpy(&val, tvb->buffer + tvb->offset, 8);
	tvb->offset += 8;
	return gg_fix64(val);
}
