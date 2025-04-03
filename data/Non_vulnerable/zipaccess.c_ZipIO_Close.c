 *****************************************************************************/
static int ZCALLBACK ZipIO_Close( void* opaque, void* stream )
{
    (void)opaque;
    stream_Delete( (stream_t*) stream );
    return 0;
}
