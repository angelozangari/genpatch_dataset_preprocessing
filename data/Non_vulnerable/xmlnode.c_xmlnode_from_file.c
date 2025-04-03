xmlnode *
xmlnode_from_file(const char *dir,const char *filename, const char *description, const char *process)
{
	gchar *filename_full;
	GError *error = NULL;
	gchar *contents = NULL;
	gsize length;
	xmlnode *node = NULL;
	g_return_val_if_fail(dir != NULL, NULL);
	purple_debug_info(process, "Reading file %s from directory %s\n",
					filename, dir);
	filename_full = g_build_filename(dir, filename, NULL);
	if (!g_file_test(filename_full, G_FILE_TEST_EXISTS))
	{
		purple_debug_info(process, "File %s does not exist (this is not "
						"necessarily an error)\n", filename_full);
		g_free(filename_full);
		return NULL;
	}
	if (!g_file_get_contents(filename_full, &contents, &length, &error))
	{
		purple_debug_error(process, "Error reading file %s: %s\n",
						 filename_full, error->message);
		g_error_free(error);
	}
	if ((contents != NULL) && (length > 0))
	{
		node = xmlnode_from_str(contents, length);
		/* If we were unable to parse the file then save its contents to a backup file */
		if (node == NULL)
		{
			gchar *filename_temp, *filename_temp_full;
			filename_temp = g_strdup_printf("%s~", filename);
			filename_temp_full = g_build_filename(dir, filename_temp, NULL);
			purple_debug_error("util", "Error parsing file %s.  Renaming old "
							 "file to %s\n", filename_full, filename_temp);
			purple_util_write_data_to_file_absolute(filename_temp_full, contents, length);
			g_free(filename_temp_full);
			g_free(filename_temp);
		}
		g_free(contents);
	}
	/* If we could not parse the file then show the user an error message */
	if (node == NULL)
	{
		gchar *title, *msg;
		title = g_strdup_printf(_("Error Reading %s"), filename);
		msg = g_strdup_printf(_("An error was encountered reading your "
					"%s.  The file has not been loaded, and the old file "
					"has been renamed to %s~."), description, filename_full);
		purple_notify_error(NULL, NULL, title, msg);
		g_free(title);
		g_free(msg);
	}
	g_free(filename_full);
	return node;
}
