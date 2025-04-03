static void
purple_theme_loader_class_init(PurpleThemeLoaderClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	GParamSpec *pspec;
	parent_class = g_type_class_peek_parent(klass);
	obj_class->get_property = purple_theme_loader_get_property;
	obj_class->set_property = purple_theme_loader_set_property;
	obj_class->finalize = purple_theme_loader_finalize;
	/* TYPE STRING (read only) */
	pspec = g_param_spec_string("type", "Type",
				    "The string representing the type of the theme",
				    NULL,
				    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property(obj_class, PROP_TYPE, pspec);
}
