*/
static char *html_to_zephyr(const char *message)
{
	zframe *frames, *new_f;
	char *ret;
	if (*message == '\0')
		return g_strdup("");
	frames = g_new(zframe, 1);
	frames->text = g_string_new("");
	frames->href = NULL;
	frames->is_href = FALSE;
	frames->enclosing = NULL;
	frames->closing = NULL;
	frames->env = "";
	frames->has_closer = FALSE;
	frames->closer_mask = 15;
	purple_debug_info("zephyr","html received %s\n",message);
	while (*message) {
		if (frames->closing && !g_ascii_strncasecmp(message, frames->closing, strlen(frames->closing))) {
			zframe *popped;
			message += strlen(frames->closing);
			popped = frames;
			frames = frames->enclosing;
			if (popped->is_href) {
				frames->href = popped->text;
			} else {
				g_string_append(frames->text, popped->env);
				if (popped->has_closer) {
					g_string_append_c(frames->text,
							  (popped->closer_mask & 1) ? '{' :
							  (popped->closer_mask & 2) ? '[' :
							  (popped->closer_mask & 4) ? '(' :
							  '<');
				}
				g_string_append(frames->text, popped->text->str);
				if (popped->href)
				{
					int text_len = strlen(popped->text->str), href_len = strlen(popped->href->str);
					if (!((text_len == href_len && !strncmp(popped->href->str, popped->text->str, text_len)) ||
					      (7 + text_len == href_len && !strncmp(popped->href->str, "http://", 7) &&
					       !strncmp(popped->href->str + 7, popped->text->str, text_len)) ||
					      (7 + text_len == href_len && !strncmp(popped->href->str, "mailto:", 7) &&
					       !strncmp(popped->href->str + 7, popped->text->str, text_len)))) {
						g_string_append(frames->text, " <");
						g_string_append(frames->text, popped->href->str);
						if (popped->closer_mask & ~8) {
							g_string_append_c(frames->text, '>');
							popped->closer_mask &= ~8;
						} else {
							g_string_append(frames->text, "@{>}");
						}
					}
					g_string_free(popped->href, TRUE);
				}
				if (popped->has_closer) {
					g_string_append_c(frames->text,
							  (popped->closer_mask & 1) ? '}' :
							  (popped->closer_mask & 2) ? ']' :
							  (popped->closer_mask & 4) ? ')' :
							  '>');
				}
				if (!popped->has_closer)
					frames->closer_mask = popped->closer_mask;
				g_string_free(popped->text, TRUE);
			}
			g_free(popped);
		} else if (*message == '<') {
			if (!g_ascii_strncasecmp(message + 1, "i>", 2)) {
				new_f = g_new(zframe, 1);
				new_f->enclosing = frames;
				new_f->text = g_string_new("");
				new_f->href = NULL;
				new_f->is_href = FALSE;
				new_f->closing = "</i>";
				new_f->env = "@i";
				new_f->has_closer = TRUE;
				new_f->closer_mask = 15;
				frames = new_f;
				message += 3;
			} else if (!g_ascii_strncasecmp(message + 1, "b>", 2)) {
				new_f = g_new(zframe, 1);
				new_f->enclosing = frames;
				new_f->text = g_string_new("");
				new_f->href = NULL;
				new_f->is_href = FALSE;
				new_f->closing = "</b>";
				new_f->env = "@b";
				new_f->has_closer = TRUE;
				new_f->closer_mask = 15;
				frames = new_f;
				message += 3;
			} else if (!g_ascii_strncasecmp(message + 1, "br>", 3)) {
				g_string_append_c(frames->text, '\n');
				message += 4;
			} else if (!g_ascii_strncasecmp(message + 1, "a href=\"", 8)) {
				message += 9;
				new_f = g_new(zframe, 1);
				new_f->enclosing = frames;
				new_f->text = g_string_new("");
				new_f->href = NULL;
				new_f->is_href = FALSE;
				new_f->closing = "</a>";
				new_f->env = "";
				new_f->has_closer = FALSE;
				new_f->closer_mask = frames->closer_mask;
				frames = new_f;
				new_f = g_new(zframe, 1);
				new_f->enclosing = frames;
				new_f->text = g_string_new("");
				new_f->href = NULL;
				new_f->is_href = TRUE;
				new_f->closing = "\">";
				new_f->has_closer = FALSE;
				new_f->closer_mask = frames->closer_mask;
				frames = new_f;
			} else if (!g_ascii_strncasecmp(message + 1, "font", 4)) {
				new_f = g_new(zframe, 1);
				new_f->enclosing = frames;
				new_f->text = g_string_new("");
				new_f->href = NULL;
				new_f->is_href = FALSE;
				new_f->closing = "</font>";
				new_f->has_closer = TRUE;
				new_f->closer_mask = 15;
				message += 5;
				while (*message == ' ')
					message++;
				if (!g_ascii_strncasecmp(message, "color=\"", 7)) {
					message += 7;
					new_f->env = "@";
					frames = new_f;
					new_f = g_new(zframe, 1);
					new_f->enclosing = frames;
					new_f->env = "@color";
					new_f->text = g_string_new("");
					new_f->href = NULL;
					new_f->is_href = FALSE;
					new_f->closing = "\">";
					new_f->has_closer = TRUE;
					new_f->closer_mask = 15;
				} else if (!g_ascii_strncasecmp(message, "face=\"", 6)) {
					message += 6;
					new_f->env = "@";
					frames = new_f;
					new_f = g_new(zframe, 1);
					new_f->enclosing = frames;
					new_f->env = "@font";
					new_f->text = g_string_new("");
					new_f->href = NULL;
					new_f->is_href = FALSE;
					new_f->closing = "\">";
					new_f->has_closer = TRUE;
					new_f->closer_mask = 15;
				} else if (!g_ascii_strncasecmp(message, "size=\"", 6)) {
					message += 6;
					if ((*message == '1') || (*message == '2')) {
						new_f->env = "@small";
					} else if ((*message == '3')
						   || (*message == '4')) {
						new_f->env = "@medium";
					} else if ((*message == '5')
						   || (*message == '6')
						   || (*message == '7')) {
						new_f->env = "@large";
					} else {
						new_f->env = "";
						new_f->has_closer = FALSE;
						new_f->closer_mask = frames->closer_mask;
					}
					message += 3;
				} else {
					/* Drop all unrecognized/misparsed font tags */
					new_f->env = "";
					new_f->has_closer = FALSE;
					new_f->closer_mask = frames->closer_mask;
					while (g_ascii_strncasecmp(message, "\">", 2) != 0) {
						message++;
					}
					if (*message != '\0')
						message += 2;
				}
				frames = new_f;
			} else {
				/* Catch all for all unrecognized/misparsed <foo> tage */
				g_string_append_c(frames->text, *message++);
			}
		} else if (*message == '@') {
			g_string_append(frames->text, "@@");
			message++;
		} else if (*message == '}') {
			if (frames->closer_mask & ~1) {
				frames->closer_mask &= ~1;
				g_string_append_c(frames->text, *message++);
			} else {
				g_string_append(frames->text, "@[}]");
				message++;
			}
		} else if (*message == ']') {
			if (frames->closer_mask & ~2) {
				frames->closer_mask &= ~2;
				g_string_append_c(frames->text, *message++);
			} else {
				g_string_append(frames->text, "@{]}");
				message++;
			}
		} else if (*message == ')') {
			if (frames->closer_mask & ~4) {
				frames->closer_mask &= ~4;
				g_string_append_c(frames->text, *message++);
			} else {
				g_string_append(frames->text, "@{)}");
				message++;
			}
		} else if (!g_ascii_strncasecmp(message, "&gt;", 4)) {
			if (frames->closer_mask & ~8) {
				frames->closer_mask &= ~8;
				g_string_append_c(frames->text, *message++);
			} else {
				g_string_append(frames->text, "@{>}");
				message += 4;
			}
		} else {
			g_string_append_c(frames->text, *message++);
		}
	}
