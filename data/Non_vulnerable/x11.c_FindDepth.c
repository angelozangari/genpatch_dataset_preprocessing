static void ResetPictures (vout_display_t *);
static const xcb_depth_t *FindDepth (const xcb_screen_t *scr,
                                     uint_fast8_t depth)
{
    xcb_depth_t *d = NULL;
    for (xcb_depth_iterator_t it = xcb_screen_allowed_depths_iterator (scr);
         it.rem > 0 && d == NULL;
         xcb_depth_next (&it))
    {
        if (it.data->depth == depth)
            d = it.data;
    }
    return d;
}
