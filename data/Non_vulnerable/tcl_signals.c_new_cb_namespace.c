}
static Tcl_Obj *new_cb_namespace ()
{
	static int cbnum;
	char name[32];
	g_snprintf (name, sizeof(name), "::purple::_callback::cb_%d", cbnum++);
	return Tcl_NewStringObj (name, -1);
}
