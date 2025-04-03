}
unsigned SetupText( filter_t *p_filter,
                           uni_char_t *psz_text_out,
                           text_style_t **pp_styles,
                           uint32_t *pi_k_dates,
                           const char *psz_text_in,
                           text_style_t *p_style,
                           uint32_t i_k_date )
{
    size_t i_string_length;
    size_t i_string_bytes;
    uni_char_t *psz_tmp = ToCharset( FREETYPE_TO_UCS, psz_text_in, &i_string_bytes );
    if( psz_tmp )
    {
        memcpy( psz_text_out, psz_tmp, i_string_bytes );
        i_string_length = i_string_bytes / sizeof( *psz_tmp );
        free( psz_tmp );
    }
    else
    {
        msg_Warn( p_filter, "failed to convert string to unicode (%s)",
                  vlc_strerror_c(errno) );
        i_string_length = 0;
    }
    if( i_string_length > 0 )
    {
        for( unsigned i = 0; i < i_string_length; i++ )
            pp_styles[i] = p_style;
    }
    else
    {
        text_style_Delete( p_style );
    }
    if( i_string_length > 0 && pi_k_dates )
    {
        for( unsigned i = 0; i < i_string_length; i++ )
            pi_k_dates[i] = i_k_date;
    }
    return i_string_length;
}
