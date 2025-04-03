static void
purple_theme_loader_set_property(GObject *obj, guint param_id, const GValue *value,
						 GParamSpec *psec)
{
	PurpleThemeLoader *loader = PURPLE_THEME_LOADER(obj);
	switch (param_id) {
		case PROP_TYPE:
			purple_theme_loader_set_type_string(loader, g_value_get_string(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, psec);
			break;
	}
}
