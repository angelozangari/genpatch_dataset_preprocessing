static gchar*
threshold_value_format_cb(GtkScale *scale, gdouble value)
{
	return g_strdup_printf ("%.*f%%", gtk_scale_get_digits(scale), value);
}
