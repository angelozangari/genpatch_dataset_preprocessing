void
oscar_show_invisible_list(PurplePluginAction *action)
{
	show_private_list(action, AIM_SSI_TYPE_DENY, _("Invisible List"),
							_("These buddies will always see you as offline"),
							_(APPEAR_OFFLINE));
}
