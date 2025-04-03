 *****************************************************************************/
static uLong ZCALLBACK ZipIO_Write( void* opaque, void* stream,
                                    const void* buf, uLong size )
{
    (void)opaque; (void)stream; (void)buf; (void)size;
    int zip_access_cannot_write_this_should_not_happen = 0;
    assert(zip_access_cannot_write_this_should_not_happen);
    return 0;
}
