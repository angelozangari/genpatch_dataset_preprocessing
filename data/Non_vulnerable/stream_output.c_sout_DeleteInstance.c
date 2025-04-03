 *****************************************************************************/
void sout_DeleteInstance( sout_instance_t * p_sout )
{
    /* remove the stream out chain */
    sout_StreamChainDelete( p_sout->p_stream, NULL );
    /* *** free all string *** */
    FREENULL( p_sout->psz_sout );
    vlc_mutex_destroy( &p_sout->lock );
    /* *** free structure *** */
    vlc_object_release( p_sout );
}
