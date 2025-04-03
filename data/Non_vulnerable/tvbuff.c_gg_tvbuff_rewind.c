 */
void gg_tvbuff_rewind(gg_tvbuff_t *tvb, size_t amount)
{
	if (!gg_tvbuff_is_valid(tvb))
		return;
	if (tvb->offset < amount) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuff_rewind() failed\n");
		tvb->valid = 0;
		return;
	}
	tvb->offset -= amount;
}
