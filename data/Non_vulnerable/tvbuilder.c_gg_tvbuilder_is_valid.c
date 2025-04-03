 */
int gg_tvbuilder_is_valid(const gg_tvbuilder_t *tvb)
{
	if (tvb == NULL)
		return 0;
	return tvb->valid;
}
