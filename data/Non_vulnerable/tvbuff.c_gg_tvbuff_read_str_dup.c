 */
void gg_tvbuff_read_str_dup(gg_tvbuff_t *tvb, char **dst)
{
	size_t offset;
	uint32_t str_len;
	char *str;
	if (!gg_tvbuff_is_valid(tvb))
		return;
	offset = tvb->offset;
	str_len = gg_tvbuff_read_packed_uint(tvb);
	if (!gg_tvbuff_is_valid(tvb) ||
		!gg_tvbuff_have_remaining(tvb, str_len))
	{
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_str_dup() failed "
			"at %" GG_SIZE_FMT ":%d\n", offset, str_len);
		return;
	}
	str = malloc(str_len + 1);
	if (str == NULL) {
		gg_debug(GG_DEBUG_ERROR, "// gg_tvbuff_read_str_dup() "
			"not enough free memory: %d + 1\n", str_len);
		tvb->valid = 0;
		return;
	}
	gg_tvbuff_read_buff_cpy(tvb, str, str_len);
	str[str_len] = '\0';
	if (*dst != NULL) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_read_str_dup() "
			"destination already filled, freeing it...\n");
		free(*dst);
	}
	*dst = str;
}
