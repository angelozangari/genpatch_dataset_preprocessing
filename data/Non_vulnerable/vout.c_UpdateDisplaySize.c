}
static void UpdateDisplaySize(vout_display_t *vd, vout_display_cfg_t *cfg)
{
    OMX_CONFIG_DISPLAYREGIONTYPE config_display;
    OMX_INIT_STRUCTURE(config_display);
    /* OMX_DISPLAY_SET_PIXEL is broadcom specific */
    if (strcmp(vd->sys->psz_component, "OMX.broadcom.video_render"))
        return;
    config_display.nPortIndex = vd->sys->port.i_port_index;
    config_display.set = OMX_DISPLAY_SET_PIXEL;
    config_display.pixel_x = cfg->display.width  * vd->fmt.i_height;
    config_display.pixel_y = cfg->display.height * vd->fmt.i_width;
    OMX_SetConfig(vd->sys->omx_handle, OMX_IndexConfigDisplayRegion, &config_display);
}
