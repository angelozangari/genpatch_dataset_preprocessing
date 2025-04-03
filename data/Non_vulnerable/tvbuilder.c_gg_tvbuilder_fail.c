 */
void gg_tvbuilder_fail(gg_tvbuilder_t *tvb, enum gg_failure_t failure)
{
	int errno_copy;
	if (tvb == NULL) {
		gg_debug(GG_DEBUG_WARNING, "// gg_tvbuilder_fail() "
			"NULL tvbuilder\n");
		return;
	}
	errno_copy = errno;
	close(tvb->gs->fd);
	tvb->gs->fd = -1;
	errno = errno_copy;
	if (tvb->ge) {
		tvb->ge->type = GG_EVENT_CONN_FAILED;
		tvb->ge->event.failure = failure;
	}
	tvb->gs->state = GG_STATE_IDLE;
	gg_tvbuilder_free(tvb);
}
