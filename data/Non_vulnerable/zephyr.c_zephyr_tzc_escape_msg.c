   escaped and do not confuse tzc: */
static char* zephyr_tzc_escape_msg(const char *message)
{
	gsize pos = 0, pos2 = 0;
	char *newmsg;
	if (message && (strlen(message) > 0)) {
		newmsg = g_new0(char,1+strlen(message)*2);
		while(pos < strlen(message)) {
			if (message[pos]=='\\') {
				newmsg[pos2]='\\';
				newmsg[pos2+1]='\\';
				pos2+=2;
			}
			else if (message[pos]=='"') {
				newmsg[pos2]='\\';
				newmsg[pos2+1]='"';
				pos2+=2;
			}
			else {
				newmsg[pos2] = message[pos];
				pos2++;
			}
			pos++;
		}
	} else {
		newmsg = g_strdup("");
	}
	/*	fprintf(stderr,"newmsg %s message %s\n",newmsg,message); */
	return newmsg;
}
