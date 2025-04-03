}
static void Delete( vlc_va_t *va )
{
    vlc_va_sys_t *sys = va->sys;
    Close( sys );
    free( sys );
}
