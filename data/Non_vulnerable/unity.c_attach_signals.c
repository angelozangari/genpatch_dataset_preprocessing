static int
attach_signals(PurpleConversation *conv)
{
	PidginConversation *gtkconv = NULL;
	guint id;
	gtkconv = PIDGIN_CONVERSATION(conv);
	if (!gtkconv)
		return 0;
	id = g_signal_connect(G_OBJECT(gtkconv->entry), "focus-in-event",
	                      G_CALLBACK(unalert_cb), conv);
	purple_conversation_set_data(conv, "unity-entry-signal", GUINT_TO_POINTER(id));
	id = g_signal_connect(G_OBJECT(gtkconv->imhtml), "focus-in-event",
	                      G_CALLBACK(unalert_cb), conv);
	purple_conversation_set_data(conv, "unity-imhtml-signal", GUINT_TO_POINTER(id));
	return 0;
}
