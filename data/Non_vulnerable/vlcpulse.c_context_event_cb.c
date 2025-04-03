}
static void context_event_cb(pa_context *c, const char *name, pa_proplist *pl,
                             void *userdata)
{
    vlc_object_t *obj = userdata;
    msg_Warn (obj, "unhandled context event \"%s\"", name);
    (void) c;
    (void) pl;
}
