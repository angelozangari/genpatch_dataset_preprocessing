 */
pa_context *vlc_pa_connect (vlc_object_t *obj, pa_threaded_mainloop **mlp)
{
    msg_Dbg (obj, "using library version %s", pa_get_library_version ());
    msg_Dbg (obj, " (compiled with version %s, protocol %u)",
             pa_get_headers_version (), PA_PROTOCOL_VERSION);
    /* Initialize main loop */
    pa_threaded_mainloop *mainloop = pa_threaded_mainloop_new ();
    if (unlikely(mainloop == NULL))
        return NULL;
    if (pa_threaded_mainloop_start (mainloop) < 0)
    {
        pa_threaded_mainloop_free (mainloop);
        return NULL;
    }
    /* Fill in context (client) properties */
    char *ua = var_InheritString (obj, "user-agent");
    pa_proplist *props = pa_proplist_new ();
    if (likely(props != NULL))
    {
        char *str;
        if (ua != NULL)
            pa_proplist_sets (props, PA_PROP_APPLICATION_NAME, ua);
        str = var_InheritString (obj, "app-id");
        if (str != NULL)
        {
            pa_proplist_sets (props, PA_PROP_APPLICATION_ID, str);
            free (str);
        }
        str = var_InheritString (obj, "app-version");
        if (str != NULL)
        {
            pa_proplist_sets (props, PA_PROP_APPLICATION_VERSION, str);
            free (str);
        }
        str = var_InheritString (obj, "app-icon-name");
        if (str != NULL)
        {
            pa_proplist_sets (props, PA_PROP_APPLICATION_ICON_NAME, str);
            free (str);
        }
        //pa_proplist_sets (props, PA_PROP_APPLICATION_LANGUAGE, _("C"));
        pa_proplist_sets (props, PA_PROP_APPLICATION_LANGUAGE,
                          setlocale (LC_MESSAGES, NULL));
        pa_proplist_setf (props, PA_PROP_APPLICATION_PROCESS_ID, "%lu",
                          (unsigned long) getpid ());
        //pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_BINARY,
        //                  PACKAGE_NAME);
        for (size_t max = sysconf (_SC_GETPW_R_SIZE_MAX), len = max % 1024 + 1024;
             len < max; len += 1024)
        {
            struct passwd pwbuf, *pw;
            char buf[len];
            if (getpwuid_r (getuid (), &pwbuf, buf, sizeof (buf), &pw) == 0)
            {
                if (pw != NULL)
                    pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_USER,
                                      pw->pw_name);
                break;
            }
        }
        for (size_t max = sysconf (_SC_HOST_NAME_MAX), len = max % 1024 + 1024;
             len < max; len += 1024)
        {
            char hostname[len];
            if (gethostname (hostname, sizeof (hostname)) == 0)
            {
                pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_HOST,
                                  hostname);
                break;
            }
        }
        const char *session = getenv ("XDG_SESSION_COOKIE");
        if (session != NULL)
        {
            pa_proplist_setf (props, PA_PROP_APPLICATION_PROCESS_MACHINE_ID,
                              "%.32s", session); /* XXX: is this valid? */
            pa_proplist_sets (props, PA_PROP_APPLICATION_PROCESS_SESSION_ID,
                              session);
        }
    }
    /* Connect to PulseAudio daemon */
    pa_context *ctx;
    pa_mainloop_api *api;
    pa_threaded_mainloop_lock (mainloop);
    api = pa_threaded_mainloop_get_api (mainloop);
    ctx = pa_context_new_with_proplist (api, ua, props);
    free (ua);
    if (props != NULL)
        pa_proplist_free (props);
    if (unlikely(ctx == NULL))
        goto fail;
    pa_context_set_state_callback (ctx, context_state_cb, mainloop);
    pa_context_set_event_callback (ctx, context_event_cb, obj);
    if (pa_context_connect (ctx, NULL, 0, NULL) < 0
     || context_wait (ctx, mainloop))
    {
        vlc_pa_error (obj, "PulseAudio server connection failure", ctx);
        pa_context_unref (ctx);
        goto fail;
    }
    msg_Dbg (obj, "connected %s to %s as client #%"PRIu32,
             pa_context_is_local (ctx) ? "locally" : "remotely",
             pa_context_get_server (ctx), pa_context_get_index (ctx));
    msg_Dbg (obj, "using protocol %"PRIu32", server protocol %"PRIu32,
             pa_context_get_protocol_version (ctx),
             pa_context_get_server_protocol_version (ctx));
    pa_threaded_mainloop_unlock (mainloop);
    *mlp = mainloop;
    return ctx;
fail:
    pa_threaded_mainloop_unlock (mainloop);
    pa_threaded_mainloop_stop (mainloop);
    pa_threaded_mainloop_free (mainloop);
    return NULL;
}
