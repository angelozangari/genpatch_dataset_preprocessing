static void
xmlnode_received_cb(PurpleConnection *gc, xmlnode **packet, gpointer null)
{
	char *str, *formatted;
	if (!console || console->gc != gc)
		return;
	str = xmlnode_to_pretty_str(*packet, NULL, 0);
	formatted = g_strdup_printf("<body bgcolor='#ffcece'><pre>%s</pre></body>", str);
	gtk_imhtml_append_text(GTK_IMHTML(console->imhtml), formatted, 0);
	g_free(formatted);
	g_free(str);
}
