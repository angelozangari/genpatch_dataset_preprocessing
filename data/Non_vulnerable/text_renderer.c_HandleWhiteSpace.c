/* Turn any multiple-whitespaces into single spaces */
void HandleWhiteSpace( char *psz_node )
{
    char *s = strpbrk( psz_node, "\t\r\n " );
    while( s )
    {
        int i_whitespace = strspn( s, "\t\r\n " );
        if( i_whitespace > 1 )
            memmove( &s[1],
                     &s[i_whitespace],
                     strlen( s ) - i_whitespace + 1 );
        *s++ = ' ';
        s = strpbrk( s, "\t\r\n " );
    }
}
