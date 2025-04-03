};
static void init_plugin(PurplePlugin * plugin)
{
	PurpleAccountOption *option;
	char *tmp = get_exposure_level();
	option = purple_account_option_bool_new(_("Use tzc"), "use_tzc", FALSE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_string_new(_("tzc command"), "tzc_command", "/usr/bin/tzc -e %s");
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_bool_new(_("Export to .anyone"), "write_anyone", FALSE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_bool_new(_("Export to .zephyr.subs"), "write_zsubs", FALSE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_bool_new(_("Import from .anyone"), "read_anyone", TRUE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_bool_new(_("Import from .zephyr.subs"), "read_zsubs", TRUE);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_string_new(_("Realm"), "realm", "");
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_string_new(_("Exposure"), "exposure_level", tmp?tmp: EXPOSE_REALMVIS);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	option = purple_account_option_string_new(_("Encoding"), "encoding", ZEPHYR_FALLBACK_CHARSET);
	prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option);
	my_protocol = plugin;
	zephyr_register_slash_commands();
}
