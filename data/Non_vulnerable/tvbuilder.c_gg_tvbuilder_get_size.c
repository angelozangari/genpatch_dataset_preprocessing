 */
size_t gg_tvbuilder_get_size(const gg_tvbuilder_t *tvb)
{
	if (!gg_tvbuilder_is_valid(tvb))
		return 0;
	return tvb->length;
}
