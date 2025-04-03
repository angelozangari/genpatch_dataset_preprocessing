 *****************************************************************************/
static int Seek( access_t *p_access, uint64_t i_pos )
{
    access_sys_t *p_sys = p_access->p_sys;
    /* might happen if called by ACCESS_SET_SEEKPOINT */
    i_pos = __MIN( i_pos, p_sys->size );
    p_access->info.i_pos = i_pos;
    p_access->info.b_eof = false;
    /* find correct chapter */
    FindSeekpoint( p_access );
    /* find correct file */
    unsigned i_file = 0;
    while( i_file < FILE_COUNT - 1 &&
           i_pos >= FILE_SIZE( i_file ) )
    {
        i_pos -= FILE_SIZE( i_file );
        i_file++;
    }
    if( !SwitchFile( p_access, i_file ) )
        return VLC_EGENERIC;
    /* adjust position within that file */
    return lseek( p_sys->fd, i_pos, SEEK_SET ) != -1 ?
        VLC_SUCCESS : VLC_EGENERIC;
}
