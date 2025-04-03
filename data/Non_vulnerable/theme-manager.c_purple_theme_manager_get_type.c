GType
purple_theme_manager_get_type(void)
{
	static GType type = 0;
	if (type == 0) {
		static const GTypeInfo info = {
			sizeof(PurpleThemeManagerClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			NULL, /* class_init */
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof(PurpleThemeManager),
			0, /* n_preallocs */
			NULL, /* instance_init */
			NULL, /* Value Table */
		};
		type = g_type_register_static(G_TYPE_OBJECT,
				"PurpleThemeManager", &info, 0);
	}
	return type;
}
