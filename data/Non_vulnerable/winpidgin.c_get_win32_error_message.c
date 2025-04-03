static LPFNSETDLLDIRECTORY MySetDllDirectory = NULL;
static const wchar_t *get_win32_error_message(DWORD err) {
	static wchar_t err_msg[512];
	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR) &err_msg, sizeof(err_msg) / sizeof(wchar_t), NULL);
	return err_msg;
}
