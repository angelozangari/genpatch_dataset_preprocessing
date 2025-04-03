static void
use_icon_theme(GtkWidget *w, GtkWidget *window)
{
	/* I don't quite understand the icon-theme stuff. For example, I don't
	 * know why PidginIconTheme needs to be abstract, or how PidginStatusIconTheme
	 * would be different from other PidginIconTheme's (e.g. PidginStockIconTheme)
	 * etc., but anyway, this works for now.
	 *
	 * Here's an interesting note: A PidginStatusIconTheme can be used for both
	 * stock and status icons. Like I said, I don't quite know how they could be
	 * different. So I am going to just keep it as it is, for now anyway, until I
	 * have the time to dig through this, or someone explains this stuff to me
	 * clearly.
	 *		-- Sad
	 */
	PidginStatusIconTheme *theme = create_icon_theme(window);
	pidgin_stock_load_status_icon_theme(PIDGIN_STATUS_ICON_THEME(theme));
	pidgin_stock_load_stock_icon_theme((PidginStockIconTheme *)theme);
	pidgin_blist_refresh(purple_get_blist());
	g_object_unref(theme);
}
