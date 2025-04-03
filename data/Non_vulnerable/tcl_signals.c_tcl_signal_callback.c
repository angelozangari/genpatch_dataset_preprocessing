}
static void *tcl_signal_callback(va_list args, struct tcl_signal_handler *handler)
{
	GString *name, *val;
	PurpleBlistNode *node;
	int i;
	void *retval = NULL;
	Tcl_Obj *cmd, *arg, *result;
	void **vals; /* Used for inout parameters */
	char ***strs;
	vals = g_new0(void *, handler->nargs);
	strs = g_new0(char **, handler->nargs);
	name = g_string_sized_new(32);
	val = g_string_sized_new(32);
	cmd = Tcl_NewListObj(0, NULL);
	Tcl_IncrRefCount(cmd);
	arg = Tcl_DuplicateObj(handler->namespace);
	Tcl_AppendStringsToObj(arg, "::cb", NULL);
	Tcl_ListObjAppendElement(handler->interp, cmd, arg);
	for (i = 0; i < handler->nargs; i++) {
		if (purple_value_is_outgoing(handler->argtypes[i]))
			g_string_printf(name, "%s::arg%d",
					Tcl_GetString(handler->namespace), i);
		switch(purple_value_get_type(handler->argtypes[i])) {
		case PURPLE_TYPE_UNKNOWN:	/* What?  I guess just pass the word ... */
			/* treat this as a pointer, but complain first */
			purple_debug(PURPLE_DEBUG_ERROR, "tcl", "unknown PurpleValue type %d\n",
				   purple_value_get_type(handler->argtypes[i]));
			/* fall through */
		case PURPLE_TYPE_POINTER:
		case PURPLE_TYPE_OBJECT:
		case PURPLE_TYPE_BOXED:
			/* These are all "pointer" types to us */
			if (purple_value_is_outgoing(handler->argtypes[i]))
				purple_debug_error("tcl", "pointer types do not currently support outgoing arguments\n");
			arg = purple_tcl_ref_new(PurpleTclRefPointer, va_arg(args, void *));
			break;
		case PURPLE_TYPE_BOOLEAN:
			if (purple_value_is_outgoing(handler->argtypes[i])) {
				vals[i] = va_arg(args, gboolean *);
				Tcl_LinkVar(handler->interp, name->str,
					    (char *)&vals[i], TCL_LINK_BOOLEAN);
				arg = Tcl_NewStringObj(name->str, -1);
			} else {
				arg = Tcl_NewBooleanObj(va_arg(args, gboolean));
			}
			break;
		case PURPLE_TYPE_CHAR:
		case PURPLE_TYPE_UCHAR:
		case PURPLE_TYPE_SHORT:
		case PURPLE_TYPE_USHORT:
		case PURPLE_TYPE_INT:
		case PURPLE_TYPE_UINT:
		case PURPLE_TYPE_LONG:
		case PURPLE_TYPE_ULONG:
		case PURPLE_TYPE_ENUM:
			/* I should really cast these individually to
			 * preserve as much information as possible ...
			 * but heh */
			if (purple_value_is_outgoing(handler->argtypes[i])) {
				vals[i] = va_arg(args, int *);
				Tcl_LinkVar(handler->interp, name->str,
					    vals[i], TCL_LINK_INT);
				arg = Tcl_NewStringObj(name->str, -1);
			} else {
				arg = Tcl_NewIntObj(va_arg(args, int));
			}
			break;
		case PURPLE_TYPE_INT64:
		case PURPLE_TYPE_UINT64:
			/* Tcl < 8.4 doesn't have wide ints, so we have ugly
			 * ifdefs in here */
			if (purple_value_is_outgoing(handler->argtypes[i])) {
				vals[i] = (void *)va_arg(args, gint64 *);
				#if (TCL_MAJOR_VERSION >= 8 && TCL_MINOR_VERSION >= 4)
				Tcl_LinkVar(handler->interp, name->str,
					    vals[i], TCL_LINK_WIDE_INT);
				#else
				/* This is going to cause weirdness at best,
				 * but what do you want ... we're losing
				 * precision */
				Tcl_LinkVar(handler->interp, name->str,
					    vals[i], TCL_LINK_INT);
				#endif /* Tcl >= 8.4 */
				arg = Tcl_NewStringObj(name->str, -1);
			} else {
				#if (TCL_MAJOR_VERSION >= 8 && TCL_MINOR_VERSION >= 4)
				arg = Tcl_NewWideIntObj(va_arg(args, gint64));
				#else
				arg = Tcl_NewIntObj((int)va_arg(args, int));
				#endif /* Tcl >= 8.4 */
			}
			break;
		case PURPLE_TYPE_STRING:
			if (purple_value_is_outgoing(handler->argtypes[i])) {
				strs[i] = va_arg(args, char **);
				if (strs[i] == NULL || *strs[i] == NULL) {
					vals[i] = ckalloc(1);
					*(char *)vals[i] = '\0';
				} else {
					size_t len = strlen(*strs[i]) + 1;
					vals[i] = ckalloc(len);
					g_strlcpy(vals[i], *strs[i], len);
				}
				Tcl_LinkVar(handler->interp, name->str,
					    (char *)&vals[i], TCL_LINK_STRING);
				arg = Tcl_NewStringObj(name->str, -1);
			} else {
				arg = Tcl_NewStringObj(va_arg(args, char *), -1);
			}
			break;
		case PURPLE_TYPE_SUBTYPE:
			switch (purple_value_get_subtype(handler->argtypes[i])) {
			case PURPLE_SUBTYPE_UNKNOWN:
				purple_debug(PURPLE_DEBUG_ERROR, "tcl", "subtype unknown\n");
				/* fall through */
			case PURPLE_SUBTYPE_ACCOUNT:
			case PURPLE_SUBTYPE_CONNECTION:
			case PURPLE_SUBTYPE_CONVERSATION:
			case PURPLE_SUBTYPE_STATUS:
			case PURPLE_SUBTYPE_PLUGIN:
			case PURPLE_SUBTYPE_XFER:
				if (purple_value_is_outgoing(handler->argtypes[i]))
					purple_debug_error("tcl", "pointer subtypes do not currently support outgoing arguments\n");
				arg = purple_tcl_ref_new(ref_type(purple_value_get_subtype(handler->argtypes[i])), va_arg(args, void *));
				break;
			case PURPLE_SUBTYPE_BLIST:
			case PURPLE_SUBTYPE_BLIST_BUDDY:
			case PURPLE_SUBTYPE_BLIST_GROUP:
			case PURPLE_SUBTYPE_BLIST_CHAT:
				/* We're going to switch again for code-deduping */
				if (purple_value_is_outgoing(handler->argtypes[i]))
					node = *va_arg(args, PurpleBlistNode **);
				else
					node = va_arg(args, PurpleBlistNode *);
				switch (purple_blist_node_get_type(node)) {
				case PURPLE_BLIST_GROUP_NODE:
					arg = Tcl_NewListObj(0, NULL);
					Tcl_ListObjAppendElement(handler->interp, arg,
								 Tcl_NewStringObj("group", -1));
					Tcl_ListObjAppendElement(handler->interp, arg,
								 Tcl_NewStringObj(purple_group_get_name((PurpleGroup *)node), -1));
					break;
				case PURPLE_BLIST_CONTACT_NODE:
					/* g_string_printf(val, "contact {%s}", Contact Name? ); */
					arg = Tcl_NewStringObj("contact", -1);
					break;
				case PURPLE_BLIST_BUDDY_NODE:
					arg = Tcl_NewListObj(0, NULL);
					Tcl_ListObjAppendElement(handler->interp, arg,
								 Tcl_NewStringObj("buddy", -1));
					Tcl_ListObjAppendElement(handler->interp, arg,
								 Tcl_NewStringObj(purple_buddy_get_name((PurpleBuddy *)node), -1));
					Tcl_ListObjAppendElement(handler->interp, arg,
								 purple_tcl_ref_new(PurpleTclRefAccount,
										    purple_buddy_get_account((PurpleBuddy *)node)));
					break;
				case PURPLE_BLIST_CHAT_NODE:
					arg = Tcl_NewListObj(0, NULL);
					Tcl_ListObjAppendElement(handler->interp, arg,
								 Tcl_NewStringObj("chat", -1));
					Tcl_ListObjAppendElement(handler->interp, arg,
								 Tcl_NewStringObj(purple_chat_get_name((PurpleChat *)node), -1));
					Tcl_ListObjAppendElement(handler->interp, arg,
								 purple_tcl_ref_new(PurpleTclRefAccount,
										  purple_chat_get_account((PurpleChat *)node)));
					break;
				case PURPLE_BLIST_OTHER_NODE:
					arg = Tcl_NewStringObj("other", -1);
					break;
				}
				break;
			}
		}
		Tcl_ListObjAppendElement(handler->interp, cmd, arg);
	}
	/* Call the friggin' procedure already */
	if (Tcl_EvalObjEx(handler->interp, cmd, TCL_EVAL_GLOBAL) != TCL_OK) {
		purple_debug(PURPLE_DEBUG_ERROR, "tcl", "error evaluating callback: %s\n",
			   Tcl_GetString(Tcl_GetObjResult(handler->interp)));
	} else {
		result = Tcl_GetObjResult(handler->interp);
		/* handle return values -- strings and words only */
		if (handler->returntype) {
			if (purple_value_get_type(handler->returntype) == PURPLE_TYPE_STRING) {
				retval = (void *)g_strdup(Tcl_GetString(result));
			} else {
				if (Tcl_GetIntFromObj(handler->interp, result, (int *)&retval) != TCL_OK) {
					purple_debug(PURPLE_DEBUG_ERROR, "tcl", "Error retrieving procedure result: %s\n",
						   Tcl_GetString(Tcl_GetObjResult(handler->interp)));
					retval = NULL;
				}
			}
		}
	}
	/* And finally clean up */
	for (i = 0; i < handler->nargs; i++) {
		g_string_printf(name, "%s::arg%d",
				Tcl_GetString(handler->namespace), i);
		if (purple_value_is_outgoing(handler->argtypes[i])
		    && purple_value_get_type(handler->argtypes[i]) != PURPLE_TYPE_SUBTYPE)
			Tcl_UnlinkVar(handler->interp, name->str);
		/* We basically only have to deal with strings on the
		 * way out */
		switch (purple_value_get_type(handler->argtypes[i])) {
		case PURPLE_TYPE_STRING:
			if (purple_value_is_outgoing(handler->argtypes[i])) {
				if (vals[i] != NULL && *(char **)vals[i] != NULL) {
					g_free(*strs[i]);
					*strs[i] = g_strdup(vals[i]);
				}
				ckfree(vals[i]);
			}
			break;
		default:
			/* nothing */
			;
		}
	}
	g_string_free(name, TRUE);
	g_string_free(val, TRUE);
	g_free(vals);
	g_free(strs);
	return retval;
}
