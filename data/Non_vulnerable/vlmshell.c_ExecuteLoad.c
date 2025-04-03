}
static int ExecuteLoad( vlm_t *p_vlm, const char *psz_path, vlm_message_t **pp_status )
{
    char *psz_url = vlc_path2uri( psz_path, NULL );
    stream_t *p_stream = stream_UrlNew( p_vlm, psz_url );
    free( psz_url );
    uint64_t i_size;
    char *psz_buffer;
    if( !p_stream )
    {
        *pp_status = vlm_MessageNew( "load", "Unable to load from file" );
        return VLC_EGENERIC;
    }
    /* FIXME needed ? */
    if( stream_Seek( p_stream, 0 ) != 0 )
    {
        stream_Delete( p_stream );
        *pp_status = vlm_MessageNew( "load", "Read file error" );
        return VLC_EGENERIC;
    }
    i_size = stream_Size( p_stream );
    if( i_size > SIZE_MAX - 1 )
        i_size = SIZE_MAX - 1;
    psz_buffer = malloc( i_size + 1 );
    if( !psz_buffer )
    {
        stream_Delete( p_stream );
        *pp_status = vlm_MessageNew( "load", "Read file error" );
        return VLC_EGENERIC;
    }
    stream_Read( p_stream, psz_buffer, i_size );
    psz_buffer[i_size] = '\0';
    stream_Delete( p_stream );
    if( Load( p_vlm, psz_buffer ) )
    {
        free( psz_buffer );
        *pp_status = vlm_MessageNew( "load", "Error while loading file" );
        return VLC_EGENERIC;
    }
    free( psz_buffer );
    *pp_status = vlm_MessageSimpleNew( "load" );
    return VLC_SUCCESS;
}
