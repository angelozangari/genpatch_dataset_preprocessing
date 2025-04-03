static gchar *
oscar_caps_to_string(guint64 caps)
{
	GString *str;
	const gchar *tmp;
	guint64 bit = 1;
	str = g_string_new("");
	if (!caps) {
		return NULL;
	} else while (bit <= OSCAR_CAPABILITY_LAST) {
		if (bit & caps) {
			switch (bit) {
			case OSCAR_CAPABILITY_BUDDYICON:
				tmp = _("Buddy Icon");
				break;
			case OSCAR_CAPABILITY_TALK:
				tmp = _("Voice");
				break;
			case OSCAR_CAPABILITY_DIRECTIM:
				tmp = _("AIM Direct IM");
				break;
			case OSCAR_CAPABILITY_CHAT:
				tmp = _("Chat");
				break;
			case OSCAR_CAPABILITY_GETFILE:
				tmp = _("Get File");
				break;
			case OSCAR_CAPABILITY_SENDFILE:
				tmp = _("Send File");
				break;
			case OSCAR_CAPABILITY_GAMES:
			case OSCAR_CAPABILITY_GAMES2:
				tmp = _("Games");
				break;
			case OSCAR_CAPABILITY_XTRAZ:
			case OSCAR_CAPABILITY_NEWCAPS:
				tmp = _("ICQ Xtraz");
				break;
			case OSCAR_CAPABILITY_ADDINS:
				tmp = _("Add-Ins");
				break;
			case OSCAR_CAPABILITY_SENDBUDDYLIST:
				tmp = _("Send Buddy List");
				break;
			case OSCAR_CAPABILITY_ICQ_DIRECT:
				tmp = _("ICQ Direct Connect");
				break;
			case OSCAR_CAPABILITY_APINFO:
				tmp = _("AP User");
				break;
			case OSCAR_CAPABILITY_ICQRTF:
				tmp = _("ICQ RTF");
				break;
			case OSCAR_CAPABILITY_EMPTY:
				tmp = _("Nihilist");
				break;
			case OSCAR_CAPABILITY_ICQSERVERRELAY:
				tmp = _("ICQ Server Relay");
				break;
			case OSCAR_CAPABILITY_UNICODEOLD:
				tmp = _("Old ICQ UTF8");
				break;
			case OSCAR_CAPABILITY_TRILLIANCRYPT:
				tmp = _("Trillian Encryption");
				break;
			case OSCAR_CAPABILITY_UNICODE:
				tmp = _("ICQ UTF8");
				break;
			case OSCAR_CAPABILITY_HIPTOP:
				tmp = _("Hiptop");
				break;
			case OSCAR_CAPABILITY_SECUREIM:
				tmp = _("Security Enabled");
				break;
			case OSCAR_CAPABILITY_VIDEO:
				tmp = _("Video Chat");
				break;
			/* Not actually sure about this one... WinAIM doesn't show anything */
			case OSCAR_CAPABILITY_ICHATAV:
				tmp = _("iChat AV");
				break;
			case OSCAR_CAPABILITY_LIVEVIDEO:
				tmp = _("Live Video");
				break;
			case OSCAR_CAPABILITY_CAMERA:
				tmp = _("Camera");
				break;
			case OSCAR_CAPABILITY_ICHAT_SCREENSHARE:
				tmp = _("Screen Sharing");
				break;
			default:
				tmp = NULL;
				break;
			}
			if (tmp)
				g_string_append_printf(str, "%s%s", (*(str->str) == '\0' ? "" : ", "), tmp);
		}
		bit <<= 1;
	}
	return g_string_free(str, FALSE);
}
