static void
status_changed_cb(PurpleSavedStatus *saved_status)
{
	MessagingMenuStatus status = MESSAGING_MENU_STATUS_AVAILABLE;
	switch (purple_savedstatus_get_type(saved_status)) {
	case PURPLE_STATUS_AVAILABLE:
	case PURPLE_STATUS_MOOD:
	case PURPLE_STATUS_TUNE:
	case PURPLE_STATUS_UNSET:
		status = MESSAGING_MENU_STATUS_AVAILABLE;
		break;
	case PURPLE_STATUS_AWAY:
	case PURPLE_STATUS_EXTENDED_AWAY:
		status = MESSAGING_MENU_STATUS_AWAY;
		break;
	case PURPLE_STATUS_INVISIBLE:
		status = MESSAGING_MENU_STATUS_INVISIBLE;
		break;
	case PURPLE_STATUS_MOBILE:
	case PURPLE_STATUS_OFFLINE:
		status = MESSAGING_MENU_STATUS_OFFLINE;
		break;
	case PURPLE_STATUS_UNAVAILABLE:
		status = MESSAGING_MENU_STATUS_BUSY;
		break;
	default:
		g_assert_not_reached();
	}
	messaging_menu_app_set_status(mmapp, status);
}
