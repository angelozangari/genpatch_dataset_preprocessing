/* Called when the server notifies us a message couldn't get sent */
static void zephyr_subscribe_failed(PurpleConnection *gc,char * z_class, char *z_instance, char * z_recipient, char* z_galaxy)
{
	gchar* subscribe_failed = g_strdup_printf(_("Attempt to subscribe to %s,%s,%s failed"), z_class, z_instance,z_recipient);
	purple_notify_error(gc,"", subscribe_failed, NULL);
	g_free(subscribe_failed);
}
