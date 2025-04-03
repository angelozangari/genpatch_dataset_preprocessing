*/
static gboolean triple_subset(zephyr_triple * zt1, zephyr_triple * zt2)
{
	if (!zt2) {
		purple_debug_error("zephyr","zt2 doesn't exist\n");
		return FALSE;
	}
	if (!zt1) {
		purple_debug_error("zephyr","zt1 doesn't exist\n");
		return FALSE;
	}
	if (!(zt1->class)) {
		purple_debug_error("zephyr","zt1c doesn't exist\n");
		return FALSE;
	}
	if (!(zt1->instance)) {
		purple_debug_error("zephyr","zt1i doesn't exist\n");
		return FALSE;
	}
	if (!(zt1->recipient)) {
		purple_debug_error("zephyr","zt1r doesn't exist\n");
		return FALSE;
	}
	if (!(zt2->class)) {
		purple_debug_error("zephyr","zt2c doesn't exist\n");
		return FALSE;
	}
	if (!(zt2->recipient)) {
		purple_debug_error("zephyr","zt2r doesn't exist\n");
		return FALSE;
	}
	if (!(zt2->instance)) {
		purple_debug_error("zephyr","zt2i doesn't exist\n");
		return FALSE;
	}
	if (g_ascii_strcasecmp(zt2->class, zt1->class)) {
		return FALSE;
	}
	if (g_ascii_strcasecmp(zt2->instance, zt1->instance) && g_ascii_strcasecmp(zt2->instance, "*")) {
		return FALSE;
	}
	if (g_ascii_strcasecmp(zt2->recipient, zt1->recipient)) {
		return FALSE;
	}
	purple_debug_info("zephyr","<%s,%s,%s> is in <%s,%s,%s>\n",zt1->class,zt1->instance,zt1->recipient,zt2->class,zt2->instance,zt2->recipient);
	return TRUE;
}
