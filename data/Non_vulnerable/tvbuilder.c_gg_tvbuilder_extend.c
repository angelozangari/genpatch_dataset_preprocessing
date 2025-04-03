 */
static char * gg_tvbuilder_extend(gg_tvbuilder_t *tvb, size_t length)
{
	size_t length_old;
	gg_tvbuilder_expected_size(tvb, length);
	if (!gg_tvbuilder_is_valid(tvb))
		return NULL;
	length_old = tvb->length;
	tvb->length += length;
	return tvb->buffer + length_old;
}
