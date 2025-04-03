static void
purple_theme_set_property(GObject *obj, guint param_id, const GValue *value,
		GParamSpec *psec)
{
	PurpleTheme *theme = PURPLE_THEME(obj);
	switch (param_id) {
		case PROP_NAME:
			purple_theme_set_name(theme, g_value_get_string(value));
			break;
		case PROP_DESCRIPTION:
			purple_theme_set_description(theme, g_value_get_string(value));
			break;
		case PROP_AUTHOR:
			purple_theme_set_author(theme, g_value_get_string(value));
			break;
		case PROP_TYPE:
			purple_theme_set_type_string(theme, g_value_get_string(value));
			break;
		case PROP_DIR:
			purple_theme_set_dir(theme, g_value_get_string(value));
			break;
		case PROP_IMAGE:
			purple_theme_set_image(theme, g_value_get_string(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, psec);
			break;
	}
}
