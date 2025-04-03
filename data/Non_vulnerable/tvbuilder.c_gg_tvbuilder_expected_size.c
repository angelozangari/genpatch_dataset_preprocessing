 */
void gg_tvbuilder_expected_size(gg_tvbuilder_t *tvb, size_t length)
{
	size_t length_new;
	char *buff_new;
	if (!gg_tvbuilder_is_valid(tvb) || length == 0)
		return;
	length_new = tvb->length + length;
	if (length_new <= tvb->alloc_length)
		return;
	if (tvb->alloc_length > 0) {
		gg_debug(GG_DEBUG_MISC, "// gg_tvbuilder_expected_size(%p, %"
			GG_SIZE_FMT ") realloc from %" GG_SIZE_FMT " to %"
			GG_SIZE_FMT "\n",
			tvb, length, tvb->alloc_length, length_new);
	}
	buff_new = realloc(tvb->buffer, length_new);
	if (buff_new != NULL) {
		tvb->buffer = buff_new;
		tvb->alloc_length = length_new;
		return;
	}
	gg_debug(GG_DEBUG_ERROR, "// gg_tvbuilder_expected_size(%p, %"
		GG_SIZE_FMT ") out of memory (new length: %" GG_SIZE_FMT
		")\n", tvb, length, length_new);
	free(tvb->buffer);
	tvb->buffer = NULL;
	tvb->length = 0;
	tvb->alloc_length = 0;
	tvb->valid = 0;
}
