}
int vlm_ControlInternal( vlm_t *p_vlm, int i_query, ... )
{
    va_list args;
    int     i_result;
    va_start( args, i_query );
    i_result = vlm_vaControlInternal( p_vlm, i_query, args );
    va_end( args );
    return i_result;
}
