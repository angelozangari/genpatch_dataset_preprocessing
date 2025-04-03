 *****************************************************************************/
static void UpdateFileSize( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    struct stat st;
    if( p_sys->size >= p_access->info.i_pos )
        return;
    /* TODO: not sure if this can happen or what to do in this case */
    if( fstat( p_sys->fd, &st ) )
        return;
    if( (uint64_t)st.st_size <= CURRENT_FILE_SIZE )
        return;
    p_sys->size -= CURRENT_FILE_SIZE;
    CURRENT_FILE_SIZE = st.st_size;
    p_sys->size += CURRENT_FILE_SIZE;
}
