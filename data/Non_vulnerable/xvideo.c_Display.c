 */
static void Display (vout_display_t *vd, picture_t *pic, subpicture_t *subpicture)
{
    vout_display_sys_t *p_sys = vd->sys;
    xcb_shm_seg_t segment = XCB_picture_GetSegment(pic);
    xcb_void_cookie_t ck;
    video_format_t fmt;
    if (!p_sys->visible)
        goto out;
    video_format_ApplyRotation(&fmt, &vd->source);
    if (segment)
        ck = xcb_xv_shm_put_image_checked (p_sys->conn, p_sys->port,
                              p_sys->window, p_sys->gc, segment, p_sys->id, 0,
                   /* Src: */ fmt.i_x_offset, fmt.i_y_offset,
                              fmt.i_visible_width, fmt.i_visible_height,
                   /* Dst: */ 0, 0, p_sys->width, p_sys->height,
                /* Memory: */ pic->p->i_pitch / pic->p->i_pixel_pitch,
                              pic->p->i_lines, false);
    else
        ck = xcb_xv_put_image_checked (p_sys->conn, p_sys->port, p_sys->window,
                          p_sys->gc, p_sys->id,
                          fmt.i_x_offset, fmt.i_y_offset,
                          fmt.i_visible_width, fmt.i_visible_height,
                          0, 0, p_sys->width, p_sys->height,
                          pic->p->i_pitch / pic->p->i_pixel_pitch,
                          pic->p->i_lines,
                          p_sys->data_size, pic->p->p_pixels);
    /* Wait for reply. See x11.c for rationale. */
    xcb_generic_error_t *e = xcb_request_check (p_sys->conn, ck);
    if (e != NULL)
    {
        msg_Dbg (vd, "%s: X11 error %d", "cannot put image", e->error_code);
        free (e);
    }
out:
    picture_Release (pic);
    (void)subpicture;
}
