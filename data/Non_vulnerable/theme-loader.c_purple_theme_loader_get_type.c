GType
purple_theme_loader_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof(PurpleThemeLoaderClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc)purple_theme_loader_class_init, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof(PurpleThemeLoader),
			0, /* n_preallocs */
			purple_theme_loader_init, /* instance_init */
			NULL, /* value table */
		};
		type = g_type_register_static(G_TYPE_OBJECT,
				"PurpleThemeLoader", &info, G_TYPE_FLAG_ABSTRACT);
	}
	return type;
}
