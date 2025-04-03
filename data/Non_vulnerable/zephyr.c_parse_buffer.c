}
static parse_tree *parse_buffer(gchar* source, gboolean do_parse) {
	parse_tree *ptree = g_new0(parse_tree,1);
	ptree->contents = NULL;
	ptree->num_children=0;
	if (do_parse) {
		unsigned int p = 0;
		while(p < strlen(source)) {
			unsigned int end;
			gchar *newstr;
			/* Eat white space: */
			if(g_ascii_isspace(source[p]) || source[p] == '\001') {
				p++;
				continue;
			}
			/* Skip comments */
			if(source[p] == ';') {
				while(source[p] != '\n' && p < strlen(source)) {
					p++;
				}
				continue;
			}
			if(source[p] == '(') {
				int nesting = 0;
				gboolean in_quote = FALSE;
				gboolean escape_next = FALSE;
				p++;
				end = p;
				while(!(source[end] == ')' && nesting == 0 && !in_quote) && end < strlen(source)) {
					if(!escape_next) {
						if(source[end] == '\\') {
							escape_next = TRUE;
						}
						if(!in_quote) {
							if(source[end] == '(') {
								nesting++;
							}
							if(source[end] == ')') {
								nesting--;
							}
						}
						if(source[end] == '"') {
							in_quote = !in_quote;
						}
					} else {
						escape_next = FALSE;
					}
					end++;
				}
				do_parse = TRUE;
			} else {
				gchar end_char;
				if(source[p] == '"') {
					end_char = '"';
					p++;
				} else {
					end_char = ' ';
				}
				do_parse = FALSE;
				end = p;
				while(source[end] != end_char && end < strlen(source)) {
					if(source[end] == '\\')
						end++;
					end++;
				}
			}
			newstr = g_new0(gchar, end+1-p);
			strncpy(newstr,source+p,end-p);
			if (ptree->num_children < MAXCHILDREN) {
				/* In case we surpass maxchildren, ignore this */
				ptree->children[ptree->num_children++] = parse_buffer( newstr, do_parse);
			} else {
				purple_debug_error("zephyr","too many children in tzc output. skipping\n");
			}
			g_free(newstr);
			p = end + 1;
		}
		return ptree;
	} else {
		/* XXX does this have to be strdup'd */
		ptree->contents = g_strdup(source);
		return ptree;
	}
}
