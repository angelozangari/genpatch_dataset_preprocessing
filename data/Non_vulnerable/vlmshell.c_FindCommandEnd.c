 */
static const char *FindCommandEnd( const char *psz_sent )
{
    unsigned char c, quote = 0;
    while( (c = *psz_sent) != '\0' )
    {
        if( !quote )
        {
            if( strchr(quotes,c) )   // opening quote
                quote = c;
            else if( isspace(c) )         // non-escaped space
                return psz_sent;
            else if( c == '\\' )
            {
                psz_sent++;         // skip escaped character
                if( *psz_sent == '\0' )
                    return psz_sent;
            }
        }
        else
        {
            if( c == quote )         // non-escaped matching quote
                quote = 0;
            else if( (quote == '"') && (c == '\\') )
            {
                psz_sent++;         // skip escaped character
                if (*psz_sent == '\0')
                    return NULL;    // error, closing quote missing
            }
        }
        psz_sent++;
    }
    // error (NULL) if we could not find a matching quote
    return quote ? NULL : psz_sent;
}
