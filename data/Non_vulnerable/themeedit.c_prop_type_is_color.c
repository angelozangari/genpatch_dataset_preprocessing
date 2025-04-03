static gboolean
prop_type_is_color(PidginBlistTheme *theme, const char *prop)
{
	PidginBlistThemeClass *klass = PIDGIN_BLIST_THEME_GET_CLASS(theme);
	GParamSpec *spec = g_object_class_find_property(G_OBJECT_CLASS(klass), prop);
	return G_IS_PARAM_SPEC_BOXED(spec);
}
