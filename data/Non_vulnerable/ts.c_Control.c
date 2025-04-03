 *****************************************************************************/
static int Control( sout_mux_t *p_mux, int i_query, va_list args )
{
    VLC_UNUSED(p_mux);
    bool *pb_bool;
    char **ppsz;
    switch( i_query )
    {
    case MUX_CAN_ADD_STREAM_WHILE_MUXING:
        pb_bool = (bool*)va_arg( args, bool * );
        *pb_bool = true;
        return VLC_SUCCESS;
    case MUX_GET_ADD_STREAM_WAIT:
        pb_bool = (bool*)va_arg( args, bool * );
        *pb_bool = false;
        return VLC_SUCCESS;
    case MUX_GET_MIME:
        ppsz = (char**)va_arg( args, char ** );
        *ppsz = strdup( "video/mp2t" );
        return VLC_SUCCESS;
    default:
        return VLC_EGENERIC;
    }
}
