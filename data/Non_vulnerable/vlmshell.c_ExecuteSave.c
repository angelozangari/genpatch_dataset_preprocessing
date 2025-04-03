}
static int ExecuteSave( vlm_t *p_vlm, const char *psz_file, vlm_message_t **pp_status )
{
    FILE *f = vlc_fopen( psz_file, "wt" );
    char *psz_save = NULL;
    if( !f )
        goto error;
    psz_save = Save( p_vlm );
    if( psz_save == NULL )
        goto error;
    if( fputs( psz_save, f ) == EOF )
        goto error;;
    if( fclose( f ) )
    {
        f = NULL;
        goto error;
    }
    free( psz_save );
    *pp_status = vlm_MessageSimpleNew( "save" );
    return VLC_SUCCESS;
error:
    free( psz_save );
    if( f )
         fclose( f );
    *pp_status = vlm_MessageNew( "save", "Unable to save to file");
    return VLC_EGENERIC;
}
