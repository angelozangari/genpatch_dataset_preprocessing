 */
void gg_tvbuff_read_buff_cpy(gg_tvbuff_t *tvb, char *buffer, size_t length)
{
	if (!gg_tvbuff_is_valid(tvb))
		return;
	if (!gg_tvbuff_have_remaining(tvb, length)) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_buff() "
			"failed at %" GG_SIZE_FMT ":%" GG_SIZE_FMT "\n",
			tvb->offset, length);
		return;
	}
	if (buffer == NULL && length > 0) {
		gg_debug(GG_DEBUG_ERROR, "// gg_tvbuff_new() "
			"invalid arguments\n");
		tvb->valid = 0;
		return;
	}
	memcpy(buffer, tvb->buffer + tvb->offset, length);
	tvb->offset += length;
}
