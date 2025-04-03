 *****************************************************************************/
static int ZCALLBACK ZipIO_Error( void* opaque, void* stream )
{
    (void)opaque;
    (void)stream;
    //msg_Dbg( p_access, "error" );
    return 0;
}
