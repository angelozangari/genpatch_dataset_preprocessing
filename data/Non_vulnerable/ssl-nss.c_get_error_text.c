}
static gchar *get_error_text(void)
{
	PRInt32 len = PR_GetErrorTextLength();
	gchar *ret = NULL;
	if (len > 0) {
		ret = g_malloc(len + 1);
		len = PR_GetErrorText(ret);
		ret[len] = '\0';
	}
	return ret;
}
