/* */
static void VoutGetDisplayCfg(vout_thread_t *vout, vout_display_cfg_t *cfg, const char *title)
{
    /* Load configuration */
    cfg->is_fullscreen = var_CreateGetBool(vout, "fullscreen")
                         || var_InheritBool(vout, "video-wallpaper");
    cfg->display.title = title;
    const int display_width = var_CreateGetInteger(vout, "width");
    const int display_height = var_CreateGetInteger(vout, "height");
    cfg->display.width   = display_width > 0  ? display_width  : 0;
    cfg->display.height  = display_height > 0 ? display_height : 0;
    cfg->is_display_filled  = var_CreateGetBool(vout, "autoscale");
    unsigned msar_num, msar_den;
    if (var_InheritURational(vout, &msar_num, &msar_den, "monitor-par") ||
        msar_num <= 0 || msar_den <= 0) {
        msar_num = 1;
        msar_den = 1;
    }
    cfg->display.sar.num = msar_num;
    cfg->display.sar.den = msar_den;
    unsigned zoom_den = 1000;
    unsigned zoom_num = zoom_den * var_CreateGetFloat(vout, "scale");
    vlc_ureduce(&zoom_num, &zoom_den, zoom_num, zoom_den, 0);
    cfg->zoom.num = zoom_num;
    cfg->zoom.den = zoom_den;
    cfg->align.vertical = VOUT_DISPLAY_ALIGN_CENTER;
    cfg->align.horizontal = VOUT_DISPLAY_ALIGN_CENTER;
    const int align_mask = var_CreateGetInteger(vout, "align");
    if (align_mask & 0x1)
        cfg->align.horizontal = VOUT_DISPLAY_ALIGN_LEFT;
    else if (align_mask & 0x2)
        cfg->align.horizontal = VOUT_DISPLAY_ALIGN_RIGHT;
    if (align_mask & 0x4)
        cfg->align.vertical = VOUT_DISPLAY_ALIGN_TOP;
    else if (align_mask & 0x8)
        cfg->align.vertical = VOUT_DISPLAY_ALIGN_BOTTOM;
}
