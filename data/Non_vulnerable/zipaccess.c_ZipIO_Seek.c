 *****************************************************************************/
static long ZCALLBACK ZipIO_Seek( void* opaque, void* stream,
                                  uLong offset, int origin )
{
    (void)opaque;
    int64_t pos = offset;
    switch( origin )
    {
        case SEEK_CUR:
            pos += stream_Tell( (stream_t*) stream );
            break;
        case SEEK_SET:
            break;
        case SEEK_END:
            pos += stream_Size( (stream_t*) stream );
            break;
        default:
            return -1;
    }
    if( pos < 0 )
        return -1;
    stream_Seek( (stream_t*) stream, pos );
    /* Note: in unzip.c, unzlocal_SearchCentralDir seeks to the end of
             the stream, which is doable but returns an error in VLC.
             That's why we always assume this was OK. FIXME */
    return 0;
}
