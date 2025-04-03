/* hash a binary file */
static int hash_from_binary_file( const char *psz_file, gcry_md_hd_t hd )
{
    uint8_t buffer[4096];
    size_t i_read;
    FILE *f = vlc_fopen( psz_file, "r" );
    if( !f )
        return -1;
    while( ( i_read = fread( buffer, 1, sizeof(buffer), f ) ) > 0 )
        gcry_md_write( hd, buffer, i_read );
    fclose( f );
    return 0;
}
