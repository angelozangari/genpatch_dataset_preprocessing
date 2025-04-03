			       const char *sig, char *opcode) ;
static const char * zephyr_get_signature(void)
{
	/* XXX add zephyr error reporting */
	const char * sig =ZGetVariable("zwrite-signature");
	if (!sig) {
		sig = g_get_real_name();
	}
	return sig;
}
