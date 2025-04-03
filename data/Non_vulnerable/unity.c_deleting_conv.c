static void
deleting_conv(PurpleConversation *conv)
{
	detach_signals(conv);
	unalert(conv);
}
