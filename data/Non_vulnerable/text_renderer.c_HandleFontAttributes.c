};
int HandleFontAttributes( xml_reader_t *p_xml_reader,
                                 font_stack_t **p_fonts )
{
    int        rv;
    char      *psz_fontname = NULL;
    uint32_t   i_font_color = 0xffffff;
    int        i_font_alpha = 255;
    uint32_t   i_karaoke_bg_color = 0x00ffffff;
    int        i_font_size  = STYLE_DEFAULT_FONT_SIZE;
    /* Default all attributes to the top font in the stack -- in case not
     * all attributes are specified in the sub-font
     */
    if( VLC_SUCCESS == PeekFont( p_fonts,
                                 &psz_fontname,
                                 &i_font_size,
                                 &i_font_color,
                                 &i_karaoke_bg_color ))
    {
        psz_fontname = strdup( psz_fontname );
    }
    i_font_alpha = (i_font_color >> 24) & 0xff;
    i_font_color &= 0x00ffffff;
    const char *name, *value;
    while( (name = xml_ReaderNextAttr( p_xml_reader, &value )) != NULL )
    {
        if( !strcasecmp( "face", name ) )
        {
            free( psz_fontname );
            psz_fontname = strdup( value );
        }
        else if( !strcasecmp( "size", name ) )
        {
            if( ( *value == '+' ) || ( *value == '-' ) )
            {
                int i_value = atoi( value );
                if( ( i_value >= -5 ) && ( i_value <= 5 ) )
                    i_font_size += ( i_value * i_font_size ) / 10;
                else if( i_value < -5 )
                    i_font_size = - i_value;
                else if( i_value > 5 )
                    i_font_size = i_value;
            }
            else
                i_font_size = atoi( value );
        }
        else if( !strcasecmp( "color", name ) )
        {
            if( value[0] == '#' )
            {
                i_font_color = strtol( value + 1, NULL, 16 );
                i_font_color &= 0x00ffffff;
            }
            else
            {
                char *end;
                uint32_t i_value = strtol( value, &end, 16 );
                if( *end == '\0' || *end == ' ' )
                    i_font_color = i_value & 0x00ffffff;
                for( int i = 0; p_html_colors[i].psz_name != NULL; i++ )
                {
                    if( !strncasecmp( value, p_html_colors[i].psz_name, strlen(p_html_colors[i].psz_name) ) )
                    {
                        i_font_color = p_html_colors[i].i_value;
                        break;
                    }
                }
            }
        }
        else if( !strcasecmp( "alpha", name ) && ( value[0] == '#' ) )
        {
            i_font_alpha = strtol( value + 1, NULL, 16 );
            i_font_alpha &= 0xff;
        }
    }
    rv = PushFont( p_fonts,
                   psz_fontname,
                   i_font_size,
                   (i_font_color & 0xffffff) | ((uint32_t)(i_font_alpha & 0xff) << 24),
                   i_karaoke_bg_color );
    free( psz_fontname );
    return rv;
}
