 */
const char *gg_tvbuff_read_str(gg_tvbuff_t *tvb, size_t *length)
{
	size_t offset;
	uint32_t str_len;
	const char *str;
	if (!gg_tvbuff_is_valid(tvb))
		return NULL;
	offset = tvb->offset;
	str_len = gg_tvbuff_read_packed_uint(tvb);
	if (!gg_tvbuff_is_valid(tvb) ||
		!gg_tvbuff_have_remaining(tvb, str_len))
	{
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_str() failed at "
			"%" GG_SIZE_FMT ":%d\n", offset, str_len);
		return NULL;
	}
	str = gg_tvbuff_read_buff(tvb, str_len);
	if (!gg_tvbuff_is_valid(tvb))
		return NULL;
	if (length != NULL)
		*length = str_len;
	if (str_len == 0)
		return NULL;
	return str;
}
