 *****************************************************************************/
int sout_AccessOutSeek( sout_access_out_t *p_access, off_t i_pos )
{
    return p_access->pf_seek( p_access, i_pos );
}
