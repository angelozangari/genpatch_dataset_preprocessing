}
char *vlc_b64_encode( const char *src )
{
    if( src )
        return vlc_b64_encode_binary( (const uint8_t*)src, strlen(src) );
    else
        return vlc_b64_encode_binary( (const uint8_t*)"", 0 );
}
