 *****************************************************************************/
static int ExecuteSyntaxError( const char *psz_cmd, vlm_message_t **pp_status )
{
    *pp_status = vlm_MessageNew( psz_cmd, "Wrong command syntax" );
    return VLC_EGENERIC;
}
