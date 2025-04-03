static void
purple_theme_get_property(GObject *obj, guint param_id, GValue *value,
		GParamSpec *psec)
{
	PurpleTheme *theme = PURPLE_THEME(obj);
	switch (param_id) {
		case PROP_NAME:
			g_value_set_string(value, purple_theme_get_name(theme));
			break;
		case PROP_DESCRIPTION:
			g_value_set_string(value, purple_theme_get_description(theme));
			break;
		case PROP_AUTHOR:
			g_value_set_string(value, purple_theme_get_author(theme));
			break;
		case PROP_TYPE:
			g_value_set_string(value, purple_theme_get_type_string(theme));
			break;
		case PROP_DIR:
			g_value_set_string(value, purple_theme_get_dir(theme));
			break;
		case PROP_IMAGE:
			g_value_set_string(value, purple_theme_get_image(theme));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, param_id, psec);
			break;
	}
}
