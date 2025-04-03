const guint purple_micro_version = PURPLE_MICRO_VERSION;
const char *purple_version_check(guint required_major, guint required_minor, guint required_micro)
{
	if (required_major > PURPLE_MAJOR_VERSION)
		return "libpurple version too old (major mismatch)";
	if (required_major < PURPLE_MAJOR_VERSION)
		return "libpurple version too new (major mismatch)";
	if (required_minor > PURPLE_MINOR_VERSION)
		return "libpurple version too old (minor mismatch)";
	if ((required_minor == PURPLE_MINOR_VERSION) && (required_micro > PURPLE_MICRO_VERSION))
		return "libpurple version too old (micro mismatch)";
	return NULL;
}
