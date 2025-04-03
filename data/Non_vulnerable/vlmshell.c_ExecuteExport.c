}
static int ExecuteExport( vlm_t *p_vlm, vlm_message_t **pp_status )
{
    char *psz_export = Save( p_vlm );
    *pp_status = vlm_MessageNew( "export", "%s", psz_export );
    free( psz_export );
    return VLC_SUCCESS;
}
