 */
void gg_tvbuff_skip(gg_tvbuff_t *tvb, size_t amount)
{
	if (!gg_tvbuff_is_valid(tvb))
		return;
	if (tvb->offset + amount > tvb->length) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_skip() failed\n");
		tvb->valid = 0;
		return;
	}
	tvb->offset += amount;
}
