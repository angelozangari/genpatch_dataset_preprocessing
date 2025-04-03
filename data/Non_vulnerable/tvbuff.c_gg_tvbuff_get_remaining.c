 */
size_t gg_tvbuff_get_remaining(const gg_tvbuff_t *tvb)
{
	if (!gg_tvbuff_is_valid(tvb))
		return 0;
	return tvb->length - tvb->offset;
}
