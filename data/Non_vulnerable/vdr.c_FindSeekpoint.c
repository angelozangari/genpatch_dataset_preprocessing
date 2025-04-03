 *****************************************************************************/
static void FindSeekpoint( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    if( !p_sys->p_marks )
        return;
    int new_seekpoint = p_sys->cur_seekpoint;
    if( p_access->info.i_pos < (uint64_t)p_sys->p_marks->
        seekpoint[p_sys->cur_seekpoint]->i_byte_offset )
    {
        /* i_pos moved backwards, start fresh */
        new_seekpoint = 0;
    }
    /* only need to check the following seekpoints */
    while( new_seekpoint + 1 < p_sys->p_marks->i_seekpoint &&
        p_access->info.i_pos >= (uint64_t)p_sys->p_marks->
        seekpoint[new_seekpoint + 1]->i_byte_offset )
    {
        new_seekpoint++;
    }
    p_sys->cur_seekpoint = new_seekpoint;
}
