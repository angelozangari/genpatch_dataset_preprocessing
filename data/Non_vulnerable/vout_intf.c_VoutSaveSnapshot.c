 */
static void VoutSaveSnapshot( vout_thread_t *p_vout )
{
    char *psz_path = var_InheritString( p_vout, "snapshot-path" );
    char *psz_format = var_InheritString( p_vout, "snapshot-format" );
    char *psz_prefix = var_InheritString( p_vout, "snapshot-prefix" );
    /* */
    picture_t *p_picture;
    block_t *p_image;
    video_format_t fmt;
    /* 500ms timeout
     * XXX it will cause trouble with low fps video (< 2fps) */
    if( vout_GetSnapshot( p_vout, &p_image, &p_picture, &fmt, psz_format, 500*1000 ) )
    {
        p_picture = NULL;
        p_image = NULL;
        goto exit;
    }
    if( !psz_path )
    {
        psz_path = vout_snapshot_GetDirectory();
        if( !psz_path )
        {
            msg_Err( p_vout, "no path specified for snapshots" );
            goto exit;
        }
    }
    vout_snapshot_save_cfg_t cfg;
    memset( &cfg, 0, sizeof(cfg) );
    cfg.is_sequential = var_InheritBool( p_vout, "snapshot-sequential" );
    cfg.sequence = var_GetInteger( p_vout, "snapshot-num" );
    cfg.path = psz_path;
    cfg.format = psz_format;
    cfg.prefix_fmt = psz_prefix;
    char *psz_filename;
    int  i_sequence;
    if (vout_snapshot_SaveImage( &psz_filename, &i_sequence,
                                 p_image, p_vout, &cfg ) )
        goto exit;
    if( cfg.is_sequential )
        var_SetInteger( p_vout, "snapshot-num", i_sequence + 1 );
    VoutOsdSnapshot( p_vout, p_picture, psz_filename );
    /* signal creation of a new snapshot file */
    var_SetString( p_vout->p_libvlc, "snapshot-file", psz_filename );
    free( psz_filename );
exit:
    if( p_image )
        block_Release( p_image );
    if( p_picture )
        picture_Release( p_picture );
    free( psz_prefix );
    free( psz_format );
    free( psz_path );
}
