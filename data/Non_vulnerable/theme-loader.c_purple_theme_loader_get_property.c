static void
purple_theme_loader_get_property(GObject *obj, guint param_id, GValue *value,
						 GParamSpec *psec)
{
	PurpleThemeLoader *theme_loader = PURPLE_THEME_LOADER(obj);
	switch (param_id) {
		case PROP_TYPE:
			g_value_set_string(value, purple_theme_loader_get_type_string(theme_loader));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, psec);
			break;
	}
}
