 */
uint32_t gg_tvbuff_read_uint32(gg_tvbuff_t *tvb)
{
	uint32_t val;
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	if (!gg_tvbuff_have_remaining(tvb, 4)) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_uint32() "
			"failed at %" GG_SIZE_FMT "\n", tvb->offset);
		return 0;
	}
	memcpy(&val, tvb->buffer + tvb->offset, 4);
	tvb->offset += 4;
	return gg_fix32(val);
}
