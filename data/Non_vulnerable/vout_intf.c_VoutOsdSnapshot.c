 */
static void VoutOsdSnapshot( vout_thread_t *p_vout, picture_t *p_pic, const char *psz_filename )
{
    msg_Dbg( p_vout, "snapshot taken (%s)", psz_filename );
    vout_OSDMessage( p_vout, SPU_DEFAULT_CHANNEL, "%s", psz_filename );
    if( var_InheritBool( p_vout, "snapshot-preview" ) )
    {
        if( VoutSnapshotPip( p_vout, p_pic ) )
            msg_Warn( p_vout, "Failed to display snapshot" );
    }
}
