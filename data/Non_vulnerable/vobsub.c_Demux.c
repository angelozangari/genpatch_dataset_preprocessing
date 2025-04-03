 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int64_t i_maxdate;
    int i_read;
    for( int i = 0; i < p_sys->i_tracks; i++ )
    {
#define tk p_sys->track[i]
        if( tk.i_current_subtitle >= tk.i_subtitles )
            continue;
        i_maxdate = p_sys->i_next_demux_date;
        if( i_maxdate <= 0 && tk.i_current_subtitle < tk.i_subtitles )
        {
            /* Should not happen */
            i_maxdate = tk.p_subtitles[tk.i_current_subtitle].i_start + 1;
        }
        while( tk.i_current_subtitle < tk.i_subtitles &&
               tk.p_subtitles[tk.i_current_subtitle].i_start < i_maxdate )
        {
            int i_pos = tk.p_subtitles[tk.i_current_subtitle].i_vobsub_location;
            block_t *p_block;
            int i_size = 0;
            /* first compute SPU size */
            if( tk.i_current_subtitle + 1 < tk.i_subtitles )
            {
                i_size = tk.p_subtitles[tk.i_current_subtitle+1].i_vobsub_location - i_pos;
            }
            if( i_size <= 0 ) i_size = 65535;   /* Invalid or EOF */
            /* Seek at the right place */
            if( stream_Seek( p_sys->p_vobsub_stream, i_pos ) )
            {
                msg_Warn( p_demux,
                          "cannot seek in the VobSub to the correct time %d", i_pos );
                tk.i_current_subtitle++;
                continue;
            }
            /* allocate a packet */
            if( ( p_block = block_Alloc( i_size ) ) == NULL )
            {
                tk.i_current_subtitle++;
                continue;
            }
            /* read data */
            i_read = stream_Read( p_sys->p_vobsub_stream, p_block->p_buffer, i_size );
            if( i_read <= 6 )
            {
                block_Release( p_block );
                tk.i_current_subtitle++;
                continue;
            }
            p_block->i_buffer = i_read;
            /* pts */
            p_block->i_pts = VLC_TS_0 + tk.p_subtitles[tk.i_current_subtitle].i_start;
            /* demux this block */
            DemuxVobSub( p_demux, p_block );
            block_Release( p_block );
            tk.i_current_subtitle++;
        }
#undef tk
    }
    /* */
    p_sys->i_next_demux_date = 0;
    return 1;
}
