static GList *
get_element_devices(const gchar *element_name)
{
	GList *ret = NULL;
	GstElement *element;
	GObjectClass *klass;
#if !GST_CHECK_VERSION(1,0,0)
	GstPropertyProbe *probe;
	const GParamSpec *pspec;
#endif
	ret = g_list_prepend(ret, (gpointer)_("Default"));
	ret = g_list_prepend(ret, "");
	if (!strcmp(element_name, "<custom>") || (*element_name == '\0')) {
		return g_list_reverse(ret);
	}
	element = gst_element_factory_make(element_name, "test");
	if(!element) {
		purple_debug_info("vvconfig", "'%s' - unable to find element\n", element_name);
		return g_list_reverse(ret);
	}
	klass = G_OBJECT_GET_CLASS (element);
	if(!klass) {
		purple_debug_info("vvconfig", "'%s' - unable to find G_Object Class\n", element_name);
		return g_list_reverse(ret);
	}
#if GST_CHECK_VERSION(1,0,0)
	purple_debug_info("vvconfig", "'%s' - gstreamer-1.0 doesn't suport "
		"property probing\n", element_name);
#else
	if (!g_object_class_find_property(klass, "device") ||
			!GST_IS_PROPERTY_PROBE(element) ||
			!(probe = GST_PROPERTY_PROBE(element)) ||
			!(pspec = gst_property_probe_get_property(probe, "device"))) {
		purple_debug_info("vvconfig", "'%s' - no device\n", element_name);
	} else {
		gsize n;
		GValueArray *array;
		/* Set autoprobe[-fps] to FALSE to avoid delays when probing. */
		if (g_object_class_find_property (klass, "autoprobe")) {
			g_object_set (G_OBJECT (element), "autoprobe", FALSE, NULL);
			if (g_object_class_find_property (klass, "autoprobe-fps")) {
				g_object_set (G_OBJECT (element), "autoprobe-fps", FALSE, NULL);
			}
		}
		array = gst_property_probe_probe_and_get_values (probe, pspec);
		if (array == NULL) {
			purple_debug_info("vvconfig", "'%s' has no devices\n", element_name);
			return g_list_reverse(ret);
		}
		for (n=0; n < array->n_values; ++n) {
			GValue *device;
			const gchar *name;
			const gchar *device_name;
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
			/* GValueArray is in gstreamer-0.10 API */
			device = g_value_array_get_nth(array, n);
G_GNUC_END_IGNORE_DEPRECATIONS
			g_object_set_property(G_OBJECT(element), "device", device);
			if (gst_element_set_state(element, GST_STATE_READY)
					!= GST_STATE_CHANGE_SUCCESS) {
				purple_debug_warning("vvconfig",
						"Error changing state of %s\n",
						element_name);
				continue;
			}
			g_object_get(G_OBJECT(element), "device-name", &name, NULL);
			device_name = g_value_get_string(device);
			if (name == NULL)
				name = _("Unknown");
			purple_debug_info("vvconfig", "Found device %s : %s for %s\n",
					device_name, name, element_name);
			ret = g_list_prepend(ret, (gpointer)name);
			ret = g_list_prepend(ret, (gpointer)device_name);
			gst_element_set_state(element, GST_STATE_NULL);
		}
	}
#endif
	gst_object_unref(element);
	return g_list_reverse(ret);
}
