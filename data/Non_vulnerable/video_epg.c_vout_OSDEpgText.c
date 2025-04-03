}
static subpicture_region_t * vout_OSDEpgText(const char *text,
                                             int x, int y,
                                             int size, uint32_t color)
{
    video_format_t fmt;
    subpicture_region_t *region;
    if (!text)
        return NULL;
    /* Create a new subpicture region */
    video_format_Init(&fmt, VLC_CODEC_TEXT);
    fmt.i_sar_num = 1;
    fmt.i_sar_den = 1;
    region = subpicture_region_New(&fmt);
    if (!region)
        return NULL;
    /* Set subpicture parameters */
    region->psz_text = strdup(text);
    region->i_align  = 0;
    region->i_x      = x;
    region->i_y      = y;
    /* Set text style */
    region->p_style = text_style_New();
    if (region->p_style) {
        region->p_style->i_font_size  = size;
        region->p_style->i_font_color = color;
        region->p_style->i_font_alpha = 0;
    }
    return region;
}
