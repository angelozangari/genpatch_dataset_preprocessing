void
oscar_show_visible_list(PurplePluginAction *action)
{
	show_private_list(action, AIM_SSI_TYPE_PERMIT, _("Visible List"),
							_("These buddies will see "
							"your status when you switch "
							"to \"Invisible\""),
							_(APPEAR_ONLINE));
}
