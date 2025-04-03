 */
void gg_tvbuilder_strip(gg_tvbuilder_t *tvb, size_t length)
{
	if (!gg_tvbuilder_is_valid(tvb))
		return;
	if (length > tvb->length) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuilder_strip() "
			"out of range\n");
		tvb->valid = 0;
		return;
	}
	tvb->length = length;
}
