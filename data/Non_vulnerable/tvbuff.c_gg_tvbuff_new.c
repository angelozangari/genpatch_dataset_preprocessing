 */
gg_tvbuff_t *gg_tvbuff_new(const char *buffer, size_t length)
{
	gg_tvbuff_t *tvb;
	tvb = malloc(sizeof(gg_tvbuff_t));
	if (tvb == NULL)
		return NULL;
	memset(tvb, 0, sizeof(gg_tvbuff_t));
	if (buffer == NULL && length > 0) {
		gg_debug(GG_DEBUG_ERROR, "// gg_tvbuff_new() "
			"invalid arguments\n");
		tvb->valid = 0;
		return tvb;
	}
	tvb->buffer = buffer;
	tvb->length = length;
	tvb->offset = 0;
	tvb->valid = 1;
	return tvb;
}
