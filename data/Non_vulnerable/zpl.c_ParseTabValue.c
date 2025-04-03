}
static char* ParseTabValue(char* psz_string)
{
    int i_len = strlen( psz_string );
    if(i_len <= 3 )
        return NULL;
    char* psz_value = calloc( i_len, 1 );
    if( ! psz_value )
        return NULL;
    sscanf( psz_string,"%*[^=]=%[^\r\t\n]", psz_value );
    return psz_value;
}
