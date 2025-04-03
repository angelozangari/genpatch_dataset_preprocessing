 */
int gg_tvbuff_match(gg_tvbuff_t *tvb, uint8_t value)
{
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	if (!gg_tvbuff_have_remaining(tvb, 1)) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_match() failed\n");
		return 0;
	}
	if (tvb->buffer[tvb->offset] != value)
		return 0;
	tvb->offset++;
	return 1;
}
