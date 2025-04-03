}
static void DisplayStat(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    VLC_UNUSED(vd);
    VLC_UNUSED(subpicture);
    if (vd->fmt.i_width*vd->fmt.i_height >= sizeof(mtime_t)) {
        mtime_t date;
        memcpy(&date, picture->p->p_pixels, sizeof(date));
        msg_Dbg(vd, "VOUT got %"PRIu64" ms offset",
                (mdate() - date) / 1000 );
    }
    picture_Release(picture);
}
