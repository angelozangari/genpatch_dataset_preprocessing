 */
int gg_tvbuff_have_remaining(gg_tvbuff_t *tvb, size_t length)
{
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	if (gg_tvbuff_get_remaining(tvb) >= length)
		return 1;
	gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_have_remaining() failed "
		"(%" GG_SIZE_FMT " < %" GG_SIZE_FMT ")\n",
		gg_tvbuff_get_remaining(tvb), length);
	tvb->valid = 0;
	return 0;
}
