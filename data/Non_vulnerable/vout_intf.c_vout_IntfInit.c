}
void vout_IntfInit( vout_thread_t *p_vout )
{
    vlc_value_t val, text, old_val;
    char *psz_buf;
    /* Create a few object variables we'll need later on */
    var_Create( p_vout, "snapshot-num", VLC_VAR_INTEGER );
    var_SetInteger( p_vout, "snapshot-num", 1 );
    var_Create( p_vout, "width", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_vout, "height", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_vout, "align", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_vout, "video-x", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_vout, "video-y", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    var_Create( p_vout, "mouse-hide-timeout",
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    /* Add variables to manage scaling video */
    var_Create( p_vout, "autoscale", VLC_VAR_BOOL | VLC_VAR_DOINHERIT
                | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Autoscale video");
    var_Change( p_vout, "autoscale", VLC_VAR_SETTEXT, &text, NULL );
    var_AddCallback( p_vout, "autoscale", AutoScaleCallback, NULL );
    var_Create( p_vout, "scale", VLC_VAR_FLOAT | VLC_VAR_DOINHERIT
                | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Scale factor");
    var_Change( p_vout, "scale", VLC_VAR_SETTEXT, &text, NULL );
    var_AddCallback( p_vout, "scale", ScaleCallback, NULL );
    /* Zoom object var */
    var_Create( p_vout, "zoom", VLC_VAR_FLOAT | VLC_VAR_ISCOMMAND |
                VLC_VAR_HASCHOICE | VLC_VAR_DOINHERIT );
    text.psz_string = _("Zoom");
    var_Change( p_vout, "zoom", VLC_VAR_SETTEXT, &text, NULL );
    old_val.f_float = var_GetFloat( p_vout, "zoom" );
    for( size_t i = 0; i < ARRAY_SIZE(p_zoom_values); i++ )
    {
        val.f_float = p_zoom_values[i].f_value;
        text.psz_string = vlc_gettext( p_zoom_values[i].psz_label );
        /* FIXME: This DELCHOICE hack corrupts the the "zoom" variable value
         * for a short time window. Same for "crop" and "aspect-ratio". */
        if( old_val.f_float == val.f_float )
            var_Change( p_vout, "zoom", VLC_VAR_DELCHOICE, &old_val, NULL );
        var_Change( p_vout, "zoom", VLC_VAR_ADDCHOICE, &val, &text );
        if( old_val.f_float == val.f_float )
            var_Change( p_vout, "zoom", VLC_VAR_SETVALUE, &old_val, NULL );
    }
    var_AddCallback( p_vout, "zoom", ZoomCallback, NULL );
    /* Crop offset vars */
    var_Create( p_vout, "crop-left", VLC_VAR_INTEGER | VLC_VAR_ISCOMMAND );
    var_Create( p_vout, "crop-top", VLC_VAR_INTEGER | VLC_VAR_ISCOMMAND );
    var_Create( p_vout, "crop-right", VLC_VAR_INTEGER | VLC_VAR_ISCOMMAND );
    var_Create( p_vout, "crop-bottom", VLC_VAR_INTEGER | VLC_VAR_ISCOMMAND );
    var_AddCallback( p_vout, "crop-left", CropBorderCallback, NULL );
    var_AddCallback( p_vout, "crop-top", CropBorderCallback, NULL );
    var_AddCallback( p_vout, "crop-right", CropBorderCallback, NULL );
    var_AddCallback( p_vout, "crop-bottom", CropBorderCallback, NULL );
    /* Crop object var */
    var_Create( p_vout, "crop", VLC_VAR_STRING | VLC_VAR_ISCOMMAND |
                VLC_VAR_HASCHOICE | VLC_VAR_DOINHERIT );
    text.psz_string = _("Crop");
    var_Change( p_vout, "crop", VLC_VAR_SETTEXT, &text, NULL );
    val.psz_string = (char*)"";
    var_Change( p_vout, "crop", VLC_VAR_DELCHOICE, &val, 0 );
    for( size_t i = 0; i < ARRAY_SIZE(p_crop_values); i++ )
    {
        val.psz_string = (char*)p_crop_values[i].psz_value;
        text.psz_string = _( p_crop_values[i].psz_label );
        var_Change( p_vout, "crop", VLC_VAR_ADDCHOICE, &val, &text );
    }
    /* Add custom crop ratios */
    psz_buf = var_CreateGetNonEmptyString( p_vout, "custom-crop-ratios" );
    if( psz_buf )
    {
        AddCustomRatios( p_vout, "crop", psz_buf );
        free( psz_buf );
    }
    var_AddCallback( p_vout, "crop", CropCallback, NULL );
    /* Monitor pixel aspect-ratio */
    var_Create( p_vout, "monitor-par", VLC_VAR_STRING | VLC_VAR_DOINHERIT );
    /* Aspect-ratio object var */
    var_Create( p_vout, "aspect-ratio", VLC_VAR_STRING | VLC_VAR_ISCOMMAND |
                VLC_VAR_HASCHOICE | VLC_VAR_DOINHERIT );
    text.psz_string = _("Aspect ratio");
    var_Change( p_vout, "aspect-ratio", VLC_VAR_SETTEXT, &text, NULL );
    val.psz_string = (char*)"";
    var_Change( p_vout, "aspect-ratio", VLC_VAR_DELCHOICE, &val, 0 );
    for( size_t i = 0; i < ARRAY_SIZE(p_aspect_ratio_values); i++ )
    {
        val.psz_string = (char*)p_aspect_ratio_values[i].psz_value;
        text.psz_string = _( p_aspect_ratio_values[i].psz_label );
        var_Change( p_vout, "aspect-ratio", VLC_VAR_ADDCHOICE, &val, &text );
    }
    /* Add custom aspect ratios */
    psz_buf = var_CreateGetNonEmptyString( p_vout, "custom-aspect-ratios" );
    if( psz_buf )
    {
        AddCustomRatios( p_vout, "aspect-ratio", psz_buf );
        free( psz_buf );
    }
    var_AddCallback( p_vout, "aspect-ratio", AspectCallback, NULL );
    /* Add a variable to indicate if the window should be on top of others */
    var_Create( p_vout, "video-on-top", VLC_VAR_BOOL | VLC_VAR_DOINHERIT
                | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Always on top");
    var_Change( p_vout, "video-on-top", VLC_VAR_SETTEXT, &text, NULL );
    var_AddCallback( p_vout, "video-on-top", AboveCallback, NULL );
    /* Add a variable to indicate if the window should be below all others */
    var_Create( p_vout, "video-wallpaper", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    var_AddCallback( p_vout, "video-wallpaper", WallPaperCallback,
                     (void *)(uintptr_t)VOUT_WINDOW_STATE_BELOW );
    /* Add a variable to indicate whether we want window decoration or not */
    var_Create( p_vout, "video-deco", VLC_VAR_BOOL | VLC_VAR_DOINHERIT );
    /* Add a fullscreen variable */
    var_Create( p_vout, "fullscreen",
                VLC_VAR_BOOL | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Fullscreen");
    var_Change( p_vout, "fullscreen", VLC_VAR_SETTEXT, &text, NULL );
    var_AddCallback( p_vout, "fullscreen", FullscreenCallback, NULL );
    /* Add a snapshot variable */
    var_Create( p_vout, "video-snapshot", VLC_VAR_VOID | VLC_VAR_ISCOMMAND );
    text.psz_string = _("Snapshot");
    var_Change( p_vout, "video-snapshot", VLC_VAR_SETTEXT, &text, NULL );
    var_AddCallback( p_vout, "video-snapshot", SnapshotCallback, NULL );
    /* Add a video-filter variable */
    var_Create( p_vout, "video-filter",
                VLC_VAR_STRING | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    var_AddCallback( p_vout, "video-filter", VideoFilterCallback, NULL );
    /* Add a sub-source variable */
    var_Create( p_vout, "sub-source",
                VLC_VAR_STRING | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    var_AddCallback( p_vout, "sub-source", SubSourceCallback, NULL );
    /* Add a sub-filter variable */
    var_Create( p_vout, "sub-filter",
                VLC_VAR_STRING | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    var_AddCallback( p_vout, "sub-filter", SubFilterCallback, NULL );
    /* Add sub-margin variable */
    var_Create( p_vout, "sub-margin",
                VLC_VAR_INTEGER | VLC_VAR_DOINHERIT | VLC_VAR_ISCOMMAND );
    var_AddCallback( p_vout, "sub-margin", SubMarginCallback, NULL );
    /* Mouse coordinates */
    var_Create( p_vout, "mouse-button-down", VLC_VAR_INTEGER );
    var_Create( p_vout, "mouse-moved", VLC_VAR_COORDS );
    var_Create( p_vout, "mouse-clicked", VLC_VAR_COORDS );
    var_Create( p_vout, "mouse-object", VLC_VAR_BOOL );
    vout_IntfReinit( p_vout );
}
