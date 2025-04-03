}
static Tcl_Obj *new_cmd_cb_namespace()
{
	char name[32];
	static int cbnum;
	g_snprintf(name, sizeof(name), "::purple::_cmd_callback::cb_%d",
	           cbnum++);
	return Tcl_NewStringObj(name, -1);
}
