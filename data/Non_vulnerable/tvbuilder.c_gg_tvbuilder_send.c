 */
int gg_tvbuilder_send(gg_tvbuilder_t *tvb, int type)
{
	int ret;
	enum gg_failure_t failure;
	if (tvb == NULL) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuilder_send() "
			"NULL tvbuilder\n");
		return 0;
	}
	if (!gg_tvbuilder_is_valid(tvb)) {
		gg_debug_session(tvb->gs, GG_DEBUG_ERROR, "// gg_tvbuilder_send() "
			"invalid buffer\n");
		ret = -1;
		failure = GG_FAILURE_INTERNAL;
	} else {
		const char *buffer = (tvb->length > 0) ? tvb->buffer : "";
		ret = gg_send_packet(tvb->gs, type, buffer, tvb->length, NULL);
		if (ret == -1) {
			failure = GG_FAILURE_WRITING;
			gg_debug_session(tvb->gs, GG_DEBUG_ERROR,
				"// gg_tvbuilder_send() "
				"sending packet %#x failed. (errno=%d, %s)\n",
				type, errno, strerror(errno));
		}
	}
	if (ret == -1) {
		gg_tvbuilder_fail(tvb, failure);
		return 0;
	}
	gg_tvbuilder_free(tvb);
	return 1;
}
