}
static PurpleStringref *ref_type(PurpleSubType type)
{
	switch (type) {
	case PURPLE_SUBTYPE_ACCOUNT:
		return PurpleTclRefAccount;
	case PURPLE_SUBTYPE_CONNECTION:
		return PurpleTclRefConnection;
	case PURPLE_SUBTYPE_CONVERSATION:
		return PurpleTclRefConversation;
	case PURPLE_SUBTYPE_PLUGIN:
		return PurpleTclRefPlugin;
	case PURPLE_SUBTYPE_STATUS:
		return PurpleTclRefStatus;
	case PURPLE_SUBTYPE_XFER:
		return PurpleTclRefXfer;
	default:
		return NULL;
	}
}
