 */
static bool CheckXVideo (vout_display_t *vd, xcb_connection_t *conn)
{
    xcb_xv_query_extension_reply_t *r;
    xcb_xv_query_extension_cookie_t ck = xcb_xv_query_extension (conn);
    bool ok = false;
    /* We need XVideo 2.2 for PutImage */
    r = xcb_xv_query_extension_reply (conn, ck, NULL);
    if (r == NULL)
        msg_Dbg (vd, "XVideo extension not available");
    else
    if (r->major != 2)
        msg_Dbg (vd, "XVideo extension v%"PRIu8".%"PRIu8" unknown",
                 r->major, r->minor);
    else
    if (r->minor < 2)
        msg_Dbg (vd, "XVideo extension v%"PRIu8".%"PRIu8" too old",
                 r->major, r->minor);
    else
    {
        msg_Dbg (vd, "using XVideo extension v%"PRIu8".%"PRIu8,
                 r->major, r->minor);
        ok = true;
    }
    free (r);
    return ok;
}
