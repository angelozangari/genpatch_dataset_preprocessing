char* zephyr_tzc_deescape_str(const char *message);
static char *zephyr_strip_local_realm(zephyr_account* zephyr,const char* user){
	/*
	  Takes in a username of the form username or username@realm
	  and returns:
	  username, if there is no realm, or the realm is the local realm
	  or:
	  username@realm if there is a realm and it is foreign
	*/
	char *tmp = g_strdup(user);
	char *at = strchr(tmp,'@');
	if (at && !g_ascii_strcasecmp(at+1,zephyr->realm)) {
		/* We're passed in a username of the form user@users-realm */
		char* tmp2;
		*at = '\0';
		tmp2 = g_strdup(tmp);
		g_free(tmp);
		return tmp2;
	}
	else {
		/* We're passed in a username of the form user or user@foreign-realm */
		return tmp;
	}
}
