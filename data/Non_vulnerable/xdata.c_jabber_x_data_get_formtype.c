gchar *
jabber_x_data_get_formtype(const xmlnode *form)
{
	xmlnode *field;
	g_return_val_if_fail(form != NULL, NULL);
	for (field = xmlnode_get_child((xmlnode *)form, "field"); field;
			field = xmlnode_get_next_twin(field)) {
		const char *var = xmlnode_get_attrib(field, "var");
		if (purple_strequal(var, "FORM_TYPE")) {
			xmlnode *value = xmlnode_get_child(field, "value");
			if (value)
				return xmlnode_get_data(value);
			else
				/* An interesting corner case... Looking for a second
				 * FORM_TYPE would be more considerate, but I'm in favor
				 * of not helping broken clients.
				 */
				return NULL;
		}
	}
	/* Erm, none found :( */
	return NULL;
}
