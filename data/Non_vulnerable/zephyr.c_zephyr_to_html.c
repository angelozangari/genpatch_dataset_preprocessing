 * "<font color=blue><i>hello</i></font>". */
static char *zephyr_to_html(const char *message)
{
	zframe *frames, *curr;
	char *ret;
	frames = g_new(zframe, 1);
	frames->text = g_string_new("");
	frames->enclosing = NULL;
	frames->closing = "";
	frames->has_closer = FALSE;
	frames->closer = NULL;
	while (*message) {
		if (*message == '@' && message[1] == '@') {
			g_string_append(frames->text, "@");
			message += 2;
		} else if (*message == '@') {
			int end;
			for (end = 1; message[end] && (isalnum(message[end]) || message[end] == '_'); end++);
			if (message[end] &&
			    (message[end] == '{' || message[end] == '[' || message[end] == '(' ||
			     !g_ascii_strncasecmp(message + end, "&lt;", 4))) {
				zframe *new_f;
				char *buf;
				buf = g_new0(char, end);
				g_snprintf(buf, end, "%s", message + 1);
				message += end;
				new_f = g_new(zframe, 1);
				new_f->enclosing = frames;
				new_f->has_closer = TRUE;
				new_f->closer = (*message == '{' ? "}" :
						 *message == '[' ? "]" :
						 *message == '(' ? ")" :
						 "&gt;");
				message += (*message == '&' ? 4 : 1);
				if (!g_ascii_strcasecmp(buf, "italic") || !g_ascii_strcasecmp(buf, "i")) {
					new_f->text = g_string_new("<i>");
					new_f->closing = "</i>";
				} else if (!g_ascii_strcasecmp(buf, "small")) {
					new_f->text = g_string_new("<font size=\"1\">");
					new_f->closing = "</font>";
				} else if (!g_ascii_strcasecmp(buf, "medium")) {
					new_f->text = g_string_new("<font size=\"3\">");
					new_f->closing = "</font>";
				} else if (!g_ascii_strcasecmp(buf, "large")) {
					new_f->text = g_string_new("<font size=\"7\">");
					new_f->closing = "</font>";
				} else if (!g_ascii_strcasecmp(buf, "bold")
					   || !g_ascii_strcasecmp(buf, "b")) {
					new_f->text = g_string_new("<b>");
					new_f->closing = "</b>";
				} else if (!g_ascii_strcasecmp(buf, "font")) {
					zframe *extra_f;
					extra_f = g_new(zframe, 1);
					extra_f->enclosing = frames;
					new_f->enclosing = extra_f;
					extra_f->text = g_string_new("");
					extra_f->has_closer = FALSE;
					extra_f->closer = frames->closer;
					extra_f->closing = "</font>";
					new_f->text = g_string_new("<font face=\"");
					new_f->closing = "\">";
				} else if (!g_ascii_strcasecmp(buf, "color")) {
					zframe *extra_f;
					extra_f = g_new(zframe, 1);
					extra_f->enclosing = frames;
					new_f->enclosing = extra_f;
					extra_f->text = g_string_new("");
					extra_f->has_closer = FALSE;
					extra_f->closer = frames->closer;
					extra_f->closing = "</font>";
					new_f->text = g_string_new("<font color=\"");
					new_f->closing = "\">";
				} else {
					new_f->text = g_string_new("");
					new_f->closing = "";
				}
				frames = new_f;
			} else {
				/* Not a formatting tag, add the character as normal. */
				g_string_append_c(frames->text, *message++);
			}
		} else if (frames->closer && !g_ascii_strncasecmp(message, frames->closer, strlen(frames->closer))) {
			zframe *popped;
			gboolean last_had_closer;
			message += strlen(frames->closer);
			if (frames->enclosing) {
				do {
					popped = frames;
					frames = frames->enclosing;
					g_string_append(frames->text, popped->text->str);
					g_string_append(frames->text, popped->closing);
					g_string_free(popped->text, TRUE);
					last_had_closer = popped->has_closer;
					g_free(popped);
				} while (frames->enclosing && !last_had_closer);
			} else {
				g_string_append_c(frames->text, *message);
			}
		} else if (*message == '\n') {
			g_string_append(frames->text, "<br>");
			message++;
		} else {
			g_string_append_c(frames->text, *message++);
		}
	}
	/* go through all the stuff that they didn't close */
	while (frames->enclosing) {
		curr = frames;
		g_string_append(frames->enclosing->text, frames->text->str);
		g_string_append(frames->enclosing->text, frames->closing);
		g_string_free(frames->text, TRUE);
		frames = frames->enclosing;
		g_free(curr);
	}
	ret = frames->text->str;
	g_string_free(frames->text, FALSE);
	g_free(frames);
	return ret;
}
