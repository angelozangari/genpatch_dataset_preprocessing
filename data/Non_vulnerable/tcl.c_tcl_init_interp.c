}
static int tcl_init_interp(Tcl_Interp *interp)
{
	char *rcfile;
	char init[] =
		"namespace eval ::purple {\n"
		"	namespace export account buddy connection conversation\n"
		"	namespace export core debug notify prefs send_im\n"
		"	namespace export signal unload\n"
		"	namespace eval _callback { }\n"
		"\n"
		"	proc conv_send { account who text } {\n"
		"		set gc [purple::account connection $account]\n"
		"		set convo [purple::conversation new $account $who]\n"
		"		set myalias [purple::account alias $account]\n"
		"\n"
		"		if {![string length $myalias]} {\n"
		"			set myalias [purple::account username $account]\n"
		"		}\n"
		"\n"
		"		purple::send_im $gc $who $text\n"
		"		purple::conversation write $convo	send $myalias $text\n"
		"	}\n"
