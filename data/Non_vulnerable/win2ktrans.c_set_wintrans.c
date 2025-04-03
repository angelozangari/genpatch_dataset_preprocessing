/* Set window transparency level */
static void set_wintrans(GtkWidget *window, int alpha, gboolean enabled,
		gboolean always_on_top) {
	HWND hWnd = GDK_WINDOW_HWND(window->window);
	LONG style = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (enabled) {
		style |= WS_EX_LAYERED;
	} else {
		style &= ~WS_EX_LAYERED;
	}
	SetWindowLong(hWnd, GWL_EXSTYLE, style);
	if (enabled) {
		SetWindowPos(hWnd,
			always_on_top ? HWND_TOPMOST : HWND_NOTOPMOST,
			0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		SetLayeredWindowAttributes(hWnd, 0, alpha, LWA_ALPHA);
	} else {
		/* Ask the window and its children to repaint */
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		RedrawWindow(hWnd, NULL, NULL,
			RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}
}
