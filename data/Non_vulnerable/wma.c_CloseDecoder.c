 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_sys_t *p_sys = ((decoder_t*)p_this)->p_sys;
    free( p_sys->p_output );
    free( p_sys );
}
