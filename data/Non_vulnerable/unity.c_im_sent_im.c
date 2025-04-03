static void
im_sent_im(PurpleAccount *account, const char *receiver, const char *message)
{
	PurpleConversation *conv = NULL;
	conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, receiver,
	                                             account);
	unalert(conv);
}
