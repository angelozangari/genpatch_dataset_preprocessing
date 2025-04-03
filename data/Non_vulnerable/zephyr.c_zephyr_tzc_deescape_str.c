}
char* zephyr_tzc_deescape_str(const char *message)
{
	gsize pos = 0, pos2 = 0;
	char *newmsg;
	if (message && (strlen(message) > 0)) {
		newmsg = g_new0(char,strlen(message)+1);
		while(pos < strlen(message)) {
			if (message[pos]=='\\') {
				pos++;
			}
			newmsg[pos2] = message[pos];
			pos++;pos2++;
		}
		newmsg[pos2]='\0';
	} else {
		newmsg = g_strdup("");
	}
	return newmsg;
}
