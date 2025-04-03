static void
purple_theme_class_init(PurpleThemeClass *klass)
{
	GObjectClass *obj_class = G_OBJECT_CLASS(klass);
	GParamSpec *pspec;
	parent_class = g_type_class_peek_parent(klass);
	obj_class->get_property = purple_theme_get_property;
	obj_class->set_property = purple_theme_set_property;
	obj_class->finalize = purple_theme_finalize;
	/* NAME */
	pspec = g_param_spec_string("name", "Name",
			"The name of the theme",
			NULL,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	g_object_class_install_property(obj_class, PROP_NAME, pspec);
	/* DESCRIPTION */
	pspec = g_param_spec_string("description", "Description",
			"The description of the theme",
			NULL,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	g_object_class_install_property(obj_class, PROP_DESCRIPTION, pspec);
	/* AUTHOR */
	pspec = g_param_spec_string("author", "Author",
			"The author of the theme",
			NULL,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	g_object_class_install_property(obj_class, PROP_AUTHOR, pspec);
	/* TYPE STRING (read only) */
	pspec = g_param_spec_string("type", "Type",
			"The string representing the type of the theme",
			NULL,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_property(obj_class, PROP_TYPE, pspec);
	/* DIRECTORY */
	pspec = g_param_spec_string("directory", "Directory",
			"The directory that contains the theme and all its files",
			NULL,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT);
	g_object_class_install_property(obj_class, PROP_DIR, pspec);
	/* PREVIEW IMAGE */
	pspec = g_param_spec_string("image", "Image",
			"A preview image of the theme",
			NULL,
			G_PARAM_READWRITE);
	g_object_class_install_property(obj_class, PROP_IMAGE, pspec);
}
