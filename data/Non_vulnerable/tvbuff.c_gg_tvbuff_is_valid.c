 */
int gg_tvbuff_is_valid(const gg_tvbuff_t *tvb)
{
	if (tvb == NULL)
		return 0;
	return tvb->valid;
}
