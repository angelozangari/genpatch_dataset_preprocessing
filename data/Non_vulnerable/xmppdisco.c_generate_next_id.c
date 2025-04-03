static char*
generate_next_id()
{
	static guint32 index = 0;
	if (index == 0) {
		do {
			index = g_random_int();
		} while (index == 0);
	}
	return g_strdup_printf("purpledisco%x", index++);
}
