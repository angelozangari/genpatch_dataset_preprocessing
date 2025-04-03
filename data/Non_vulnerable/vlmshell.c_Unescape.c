 */
static int Unescape( char *out, const char *in )
{
    unsigned char c, quote = 0;
    bool param = false;
    while( (c = *in++) != '\0' )
    {
        // Don't escape the end of the string if we find a '#'
        // that's the begining of a vlc command
        // TODO: find a better solution
        if( ( c == '#' && !quote ) || param )
        {
            param = true;
            *out++ = c;
            continue;
        }
        if( !quote )
        {
            if (strchr(quotes,c))   // opening quote
            {
                quote = c;
                continue;
            }
            else if( c == '\\' )
            {
                switch (c = *in++)
                {
                    case '"':
                    case '\'':
                    case '\\':
                        *out++ = c;
                        continue;
                    case '\0':
                        *out = '\0';
                        return 0;
                }
                if( isspace(c) )
                {
                    *out++ = c;
                    continue;
                }
                /* None of the special cases - copy the backslash */
                *out++ = '\\';
            }
        }
        else
        {
            if( c == quote )         // non-escaped matching quote
            {
                quote = 0;
                continue;
            }
            if( (quote == '"') && (c == '\\') )
            {
                switch( c = *in++ )
                {
                    case '"':
                    case '\\':
                        *out++ = c;
                        continue;
                    case '\0':   // should never happen
                        *out = '\0';
                        return -1;
                }
                /* None of the special cases - copy the backslash */
                *out++ = '\\';
            }
        }
        *out++ = c;
    }
    *out = '\0';
    return 0;
}
