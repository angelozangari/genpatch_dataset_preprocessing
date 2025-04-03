 */
gg_tvbuilder_t *gg_tvbuilder_new(struct gg_session *gs, struct gg_event *ge)
{
	gg_tvbuilder_t *tvb;
	tvb = malloc(sizeof(gg_tvbuilder_t));
	if (tvb == NULL)
		return NULL;
	memset(tvb, 0, sizeof(gg_tvbuilder_t));
	if (gs == NULL) {
		gg_debug(GG_DEBUG_ERROR, "// gg_tvbuilder_new() "
			"invalid arguments\n");
		tvb->valid = 0;
		return tvb;
	}
	tvb->buffer = NULL;
	tvb->length = 0;
	tvb->alloc_length = 0;
	tvb->valid = 1;
	tvb->gs = gs;
	tvb->ge = ge;
	return tvb;
}
