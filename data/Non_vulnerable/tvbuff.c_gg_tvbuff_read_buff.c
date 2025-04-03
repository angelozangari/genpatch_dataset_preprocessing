 */
const char *gg_tvbuff_read_buff(gg_tvbuff_t *tvb, size_t length)
{
	const char *buff;
	if (!gg_tvbuff_is_valid(tvb))
		return NULL;
	if (!gg_tvbuff_have_remaining(tvb, length)) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_get_buff() "
			"failed at %" GG_SIZE_FMT ":%" GG_SIZE_FMT "\n",
			tvb->offset, length);
		return NULL;
	}
	buff = tvb->buffer + tvb->offset;
	tvb->offset += length;
	return buff;
}
