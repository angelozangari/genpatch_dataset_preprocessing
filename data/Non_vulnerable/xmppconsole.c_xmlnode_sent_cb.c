static void
xmlnode_sent_cb(PurpleConnection *gc, char **packet, gpointer null)
{
	char *str;
	char *formatted;
	xmlnode *node;
	if (!console || console->gc != gc)
		return;
	node = xmlnode_from_str(*packet, -1);
	if (!node)
		return;
	str = xmlnode_to_pretty_str(node, NULL, 0);
	formatted = g_strdup_printf("<body bgcolor='#dcecc4'><pre>%s</pre></body>", str);
	gtk_imhtml_append_text(GTK_IMHTML(console->imhtml), formatted, 0);
	g_free(formatted);
	g_free(str);
	xmlnode_free(node);
}
