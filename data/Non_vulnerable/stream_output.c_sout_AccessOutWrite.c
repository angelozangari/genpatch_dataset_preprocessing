 *****************************************************************************/
ssize_t sout_AccessOutWrite( sout_access_out_t *p_access, block_t *p_buffer )
{
    return p_access->pf_write( p_access, p_buffer );
}
