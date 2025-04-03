static void
messaging_menu_status_changed(MessagingMenuApp *mmapp,
                              MessagingMenuStatus mm_status, gpointer user_data)
{
	PurpleSavedStatus *saved_status;
	PurpleStatusPrimitive primitive = PURPLE_STATUS_UNSET;
	switch (mm_status) {
	case MESSAGING_MENU_STATUS_AVAILABLE:
		primitive = PURPLE_STATUS_AVAILABLE;
		break;
	case MESSAGING_MENU_STATUS_AWAY:
		primitive = PURPLE_STATUS_AWAY;
		break;
	case MESSAGING_MENU_STATUS_BUSY:
		primitive = PURPLE_STATUS_UNAVAILABLE;
		break;
	case MESSAGING_MENU_STATUS_INVISIBLE:
		primitive = PURPLE_STATUS_INVISIBLE;
		break;
	case MESSAGING_MENU_STATUS_OFFLINE:
		primitive = PURPLE_STATUS_OFFLINE;
		break;
	default:
		g_assert_not_reached();
	}
	saved_status = purple_savedstatus_find_transient_by_type_and_message(primitive, NULL);
	if (saved_status == NULL)
		saved_status = create_transient_status(primitive, NULL);
	purple_savedstatus_activate(saved_status);
}
