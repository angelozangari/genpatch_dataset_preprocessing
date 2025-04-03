 *****************************************************************************/
static uLong ZCALLBACK ZipIO_Read( void* opaque, void* stream,
                                   void* buf, uLong size )
{
    (void)opaque;
    //access_t *p_access = (access_t*) opaque;
    //msg_Dbg(p_access, "read %d", size);
    return stream_Read( (stream_t*) stream, buf, size );
}
