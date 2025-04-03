GType
purple_theme_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof(PurpleThemeClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc)purple_theme_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof(PurpleTheme),
			0, /* n_preallocs */
			purple_theme_init, /* instance_init */
			NULL, /* value table */
		};
		type = g_type_register_static (G_TYPE_OBJECT,
				"PurpleTheme", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
