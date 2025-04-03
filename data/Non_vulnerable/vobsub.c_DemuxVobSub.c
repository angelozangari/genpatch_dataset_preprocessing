}
static int DemuxVobSub( demux_t *p_demux, block_t *p_bk )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    uint8_t     *p = p_bk->p_buffer;
    uint8_t     *p_end = &p_bk->p_buffer[p_bk->i_buffer];
    int i;
    while( p + 6 < p_end )
    {
        int i_size = ps_pkt_size( p, p_end - p );
        block_t *p_pkt;
        int      i_id;
        int      i_spu;
        if( i_size <= 0 )
            break;
        if( i_size > p_end - p )
        {
            msg_Warn( p_demux, "broken PES size" );
            break;
        }
        if( p[0] != 0 || p[1] != 0 || p[2] != 0x01 )
        {
            msg_Warn( p_demux, "invalid PES" );
            break;
        }
        if( p[3] != 0xbd )
        {
            /* msg_Dbg( p_demux, "we don't need these ps packets (id=0x1%2.2x)", p[3] ); */
            p += i_size;
            continue;
        }
        /* Create a block */
        p_pkt = block_Alloc( i_size );
        if( unlikely(p_pkt == NULL) )
            break;
        memcpy( p_pkt->p_buffer, p, i_size);
        p += i_size;
        i_id = ps_pkt_id( p_pkt );
        if( (i_id&0xffe0) != 0xbd20 ||
            ps_pkt_parse_pes( p_pkt, 1 ) )
        {
            block_Release( p_pkt );
            continue;
        }
        i_spu = i_id&0x1f;
        /* msg_Dbg( p_demux, "SPU track %d size %d", i_spu, i_size ); */
        for( i = 0; i < p_sys->i_tracks; i++ )
        {
            vobsub_track_t *p_tk = &p_sys->track[i];
            p_pkt->i_dts = p_pkt->i_pts = p_bk->i_pts;
            p_pkt->i_length = 0;
            if( p_tk->p_es && p_tk->i_track_id == i_spu )
            {
                es_out_Send( p_demux->out, p_tk->p_es, p_pkt );
                p_bk->i_pts = VLC_TS_INVALID;     /*only first packet has a pts */
                break;
            }
        }
        if( i >= p_sys->i_tracks )
        {
            block_Release( p_pkt );
        }
    }
    return VLC_SUCCESS;
}
