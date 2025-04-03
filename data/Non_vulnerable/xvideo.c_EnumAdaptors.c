}
static int EnumAdaptors (vlc_object_t *obj, const char *var,
                         int64_t **vp, char ***tp)
{
    /* Connect to X */
    char *display = var_InheritString (obj, "x11-display");
    xcb_connection_t *conn;
    int snum;
    conn = xcb_connect (display, &snum);
    free (display);
    if (xcb_connection_has_error (conn) /*== NULL*/)
        return -1;
    /* Find configured screen */
    const xcb_setup_t *setup = xcb_get_setup (conn);
    const xcb_screen_t *scr = NULL;
    for (xcb_screen_iterator_t i = xcb_setup_roots_iterator (setup);
         i.rem > 0; xcb_screen_next (&i))
    {
        if (snum == 0)
        {
            scr = i.data;
            break;
        }
        snum--;
    }
    if (scr == NULL)
    {
        xcb_disconnect (conn);
        return -1;
    }
    xcb_xv_query_adaptors_reply_t *adaptors =
        xcb_xv_query_adaptors_reply (conn,
            xcb_xv_query_adaptors (conn, scr->root), NULL);
    xcb_disconnect (conn);
    if (adaptors == NULL)
        return -1;
    xcb_xv_adaptor_info_iterator_t it;
    size_t n = 0;
    for (it = xcb_xv_query_adaptors_info_iterator (adaptors);
         it.rem > 0;
         xcb_xv_adaptor_info_next (&it))
    {
        const xcb_xv_adaptor_info_t *a = it.data;
        if ((a->type & XCB_XV_TYPE_INPUT_MASK)
         && (a->type & XCB_XV_TYPE_IMAGE_MASK))
            n++;
    }
    int64_t *values = xmalloc ((n + 1) * sizeof (*values));
    char **texts = xmalloc ((n + 1) * sizeof (*texts));
    *vp = values;
    *tp = texts;
    *(values++) = -1;
    *(texts++) = strdup (N_("Auto"));
    for (it = xcb_xv_query_adaptors_info_iterator (adaptors), n = -1;
         it.rem > 0;
         xcb_xv_adaptor_info_next (&it))
    {
        const xcb_xv_adaptor_info_t *a = it.data;
        n++;
        if (!(a->type & XCB_XV_TYPE_INPUT_MASK)
         || !(a->type & XCB_XV_TYPE_IMAGE_MASK))
            continue;
        *(values++) = n;
        *(texts++) = strndup (xcb_xv_adaptor_info_name (a), a->name_size);
    }
    free (adaptors);
    (void) obj; (void) var;
    return values - *vp;
}
