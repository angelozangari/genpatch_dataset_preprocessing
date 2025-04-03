 *****************************************************************************/
static void* ZCALLBACK ZipIO_Open( void* opaque, const char* file, int mode )
{
    assert(opaque != NULL);
    assert(mode == (ZLIB_FILEFUNC_MODE_READ | ZLIB_FILEFUNC_MODE_EXISTING));
    access_t *p_access = (access_t*) opaque;
    char *fileUri = malloc( strlen(file) + 8 );
    if( unlikely( !fileUri ) )
        return NULL;
    if( !strstr( file, "://" ) )
    {
        strcpy( fileUri, "file://" );
        strcat( fileUri, file );
    }
    else
    {
        strcpy( fileUri, file );
    }
    stream_t *s = stream_UrlNew( p_access, fileUri );
    free( fileUri );
    return s;
}
