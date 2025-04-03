static GSList *callbacks = NULL;
static void close_stun_conn(struct stun_conn *sc) {
	if (sc->incb)
		purple_input_remove(sc->incb);
	if (sc->timeout)
		purple_timeout_remove(sc->timeout);
	if (sc->fd)
		close(sc->fd);
	g_free(sc);
}
