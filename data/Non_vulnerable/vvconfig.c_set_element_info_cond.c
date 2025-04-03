static void
set_element_info_cond(PurpleMediaElementInfo *old_info,
		PurpleMediaElementInfo *new_info, const gchar *id)
{
	gchar *element_id = purple_media_element_info_get_id(old_info);
	if (!strcmp(element_id, id))
		purple_media_manager_set_active_element(
				purple_media_manager_get(), new_info);
	g_free(element_id);
}
