 */
uint8_t gg_tvbuff_read_uint8(gg_tvbuff_t *tvb)
{
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	if (!gg_tvbuff_have_remaining(tvb, 1)) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_uint8() "
			"failed at %" GG_SIZE_FMT "\n", tvb->offset);
		return 0;
	}
	return tvb->buffer[tvb->offset++];
}
