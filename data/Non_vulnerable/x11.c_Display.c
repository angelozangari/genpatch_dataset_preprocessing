 */
static void Display (vout_display_t *vd, picture_t *pic, subpicture_t *subpicture)
{
    vout_display_sys_t *sys = vd->sys;
    xcb_shm_seg_t segment = XCB_picture_GetSegment(pic);
    xcb_void_cookie_t ck;
    if (!sys->visible)
        goto out;
    if (segment != 0)
        ck = xcb_shm_put_image_checked (sys->conn, sys->window, sys->gc,
          /* real width */ pic->p->i_pitch / pic->p->i_pixel_pitch,
         /* real height */ pic->p->i_lines,
                   /* x */ vd->fmt.i_x_offset,
                   /* y */ vd->fmt.i_y_offset,
               /* width */ vd->fmt.i_visible_width,
              /* height */ vd->fmt.i_visible_height,
                           0, 0, sys->depth, XCB_IMAGE_FORMAT_Z_PIXMAP,
                           0, segment, 0);
    else
    {
        const size_t offset = vd->fmt.i_y_offset * pic->p->i_pitch;
        const unsigned lines = pic->p->i_lines - vd->fmt.i_y_offset;
        ck = xcb_put_image_checked (sys->conn, XCB_IMAGE_FORMAT_Z_PIXMAP,
                       sys->window, sys->gc,
                       pic->p->i_pitch / pic->p->i_pixel_pitch,
                       lines, -vd->fmt.i_x_offset, 0, 0, sys->depth,
                       pic->p->i_pitch * lines, pic->p->p_pixels + offset);
    }
    /* Wait for reply. This makes sure that the X server gets CPU time to
     * display the picture. xcb_flush() is *not* sufficient: especially with
     * shared memory the PUT requests are so short that many of them can fit in
     * X11 socket output buffer before the kernel preempts VLC. */
    xcb_generic_error_t *e = xcb_request_check (sys->conn, ck);
    if (e != NULL)
    {
        msg_Dbg (vd, "%s: X11 error %d", "cannot put image", e->error_code);
        free (e);
    }
    /* FIXME might be WAY better to wait in some case (be carefull with
     * VOUT_DISPLAY_RESET_PICTURES if done) + does not work with
     * vout_display wrapper. */
out:
    picture_Release (pic);
    (void)subpicture;
}
