/*** Initialization ***/
void vlc_threads_setup (libvlc_int_t *p_libvlc)
{
    SelectClockSource (VLC_OBJECT(p_libvlc));
}
