 *****************************************************************************/
static long ZCALLBACK ZipIO_Tell( void* opaque, void* stream )
{
    (void)opaque;
    int64_t i64_tell = stream_Tell( (stream_t*) stream );
    //access_t *p_access = (access_t*) opaque;
    //msg_Dbg(p_access, "tell %" PRIu64, i64_tell);
    return (long)i64_tell;
}
