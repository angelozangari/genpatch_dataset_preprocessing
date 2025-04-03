}
static gboolean tcl_probe_plugin(PurplePlugin *plugin)
{
	PurplePluginInfo *info;
	Tcl_Interp *interp;
	Tcl_Parse parse;
	Tcl_Obj *result, **listitems;
	char *buf;
	const char *next;
	int found = 0, err = 0, nelems;
	gsize len;
	gboolean status = FALSE;
	if (!g_file_get_contents(plugin->path, &buf, &len, NULL)) {
		purple_debug(PURPLE_DEBUG_INFO, "tcl", "Error opening plugin %s\n",
			     plugin->path);
		return FALSE;
	}
	if ((interp = tcl_create_interp()) == NULL) {
		return FALSE;
	}
	next = buf;
	do {
		if (Tcl_ParseCommand(interp, next, len, 0, &parse) == TCL_ERROR) {
			purple_debug(PURPLE_DEBUG_ERROR, "tcl", "parse error in %s: %s\n", plugin->path,
				   Tcl_GetString(Tcl_GetObjResult(interp)));
			err = 1;
			break;
		}
		if (parse.tokenPtr[0].type == TCL_TOKEN_SIMPLE_WORD
		    && !strncmp(parse.tokenPtr[0].start, "proc", parse.tokenPtr[0].size)) {
			if (!strncmp(parse.tokenPtr[2].start, "plugin_init", parse.tokenPtr[2].size)) {
				if (Tcl_EvalEx(interp, parse.commandStart, parse.commandSize, TCL_EVAL_GLOBAL) != TCL_OK) {
					Tcl_FreeParse(&parse);
					break;
				}
				found = 1;
				/* We'll continue parsing the file, just in case */
			}
		}
		len -= (parse.commandStart + parse.commandSize) - next;
		next = parse.commandStart + parse.commandSize;
		Tcl_FreeParse(&parse);
	} while (len);
	if (found && !err) {
		if (Tcl_EvalEx(interp, "plugin_init", -1, TCL_EVAL_GLOBAL) == TCL_OK) {
			result = Tcl_GetObjResult(interp);
			if (Tcl_ListObjGetElements(interp, result, &nelems, &listitems) == TCL_OK) {
				if ((nelems == 6) || (nelems == 7)) {
					info = g_new0(PurplePluginInfo, 1);
					info->magic = PURPLE_PLUGIN_MAGIC;
					info->major_version = PURPLE_MAJOR_VERSION;
					info->minor_version = PURPLE_MINOR_VERSION;
					info->type = PURPLE_PLUGIN_STANDARD;
					info->dependencies = g_list_append(info->dependencies, "core-tcl");
					info->name = g_strdup(Tcl_GetString(listitems[0]));
					info->version = g_strdup(Tcl_GetString(listitems[1]));
					info->summary = g_strdup(Tcl_GetString(listitems[2]));
					info->description = g_strdup(Tcl_GetString(listitems[3]));
					info->author = g_strdup(Tcl_GetString(listitems[4]));
					info->homepage = g_strdup(Tcl_GetString(listitems[5]));
					if (nelems == 6)
						info->id = g_strdup_printf("tcl-%s", Tcl_GetString(listitems[0]));
					else if (nelems == 7)
						info->id = g_strdup_printf("tcl-%s", Tcl_GetString(listitems[6]));
					plugin->info = info;
					if (purple_plugin_register(plugin))
						status = TRUE;
				}
			}
		}
	}
	Tcl_DeleteInterp(interp);
	g_free(buf);
	return status;
}
