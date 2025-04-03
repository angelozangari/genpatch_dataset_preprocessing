}
static int zephyr_send_message(zephyr_account *zephyr,char* zclass, char* instance, char* recipient, const char *im,
			       const char *sig, char *opcode)
{
	/* (From the tzc source)
	 * emacs sends something of the form:
	 * ((class . "MESSAGE")
	 *  (auth . t)
	 *  (recipients ("PERSONAL" . "bovik") ("test" . ""))
	 *  (sender . "bovik")
	 *  (message . ("Harry Bovik" "my zgram"))
	 * )
	 */
	char *html_buf;
	char *html_buf2;
	html_buf = html_to_zephyr(im);
	html_buf2 = purple_unescape_html(html_buf);
	if(use_tzc(zephyr)) {
		size_t len;
		size_t result;
		char* zsendstr;
		/* CMU cclub tzc doesn't grok opcodes for now  */
		char* tzc_sig = zephyr_tzc_escape_msg(sig);
		char *tzc_body = zephyr_tzc_escape_msg(html_buf2);
		zsendstr = g_strdup_printf("((tzcfodder . send) (class . \"%s\") (auth . t) (recipients (\"%s\" . \"%s\")) (message . (\"%s\" \"%s\"))	) \n",
					   zclass, instance, recipient, tzc_sig, tzc_body);
		/*		fprintf(stderr,"zsendstr = %s\n",zsendstr); */
		len = strlen(zsendstr);
		result = write(zephyr->totzc[ZEPHYR_FD_WRITE], zsendstr, len);
		if (result != len) {
			g_free(zsendstr);
			g_free(html_buf2);
			g_free(html_buf);
			return errno;
		}
		g_free(zsendstr);
	} else if (use_zeph02(zephyr)) {
		ZNotice_t notice;
		char *buf = g_strdup_printf("%s%c%s", sig, '\0', html_buf2);
		memset((char *)&notice, 0, sizeof(notice));
		notice.z_kind = ACKED;
		notice.z_port = 0;
		notice.z_opcode = "";
		notice.z_class = zclass;
		notice.z_class_inst = instance;
		notice.z_recipient = recipient;
		notice.z_sender = 0;
		notice.z_default_format = "Class $class, Instance $instance:\n" "To: @bold($recipient) at $time $date\n" "From: @bold($1) <$sender>\n\n$2";
		notice.z_message_len = strlen(html_buf2) + strlen(sig) + 2;
		notice.z_message = buf;
		notice.z_opcode = g_strdup(opcode);
		purple_debug_info("zephyr","About to send notice\n");
		if (! ZSendNotice(&notice, ZAUTH) == ZERR_NONE) {
			/* XXX handle errors here */
			g_free(buf);
			g_free(html_buf2);
			g_free(html_buf);
			return 0;
		}
		purple_debug_info("zephyr","notice sent\n");
		g_free(buf);
	}
	g_free(html_buf2);
	g_free(html_buf);
	return 1;
}
