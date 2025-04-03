 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    xa_header_t p_xa;
    const uint8_t *p_buf;
    /* XA file heuristic */
    if( stream_Peek( p_demux->s, &p_buf, sizeof( p_xa ) )
            < (signed)sizeof( p_xa ) )
        return VLC_EGENERIC;
    memcpy( &p_xa, p_buf, sizeof( p_xa ) );
    if( ( strncmp( p_xa.xa_id, "XAI", 4 )
       && strncmp( p_xa.xa_id, "XAJ", 4 ) )
     || ( GetWLE( &p_xa.wFormatTag  ) != 0x0001)
     || ( GetWLE( &p_xa.wBitsPerSample ) != 16) )
        return VLC_EGENERIC;
    p_sys = malloc( sizeof( demux_sys_t ) );
    if( unlikely( p_sys == NULL ) )
        return VLC_ENOMEM;
    p_demux->pf_demux   = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys      = p_sys;
    p_sys->p_es         = NULL;
    /* skip XA header -- cannot fail */
    stream_Read( p_demux->s, NULL, sizeof( p_xa ) );
    es_format_t fmt;
    es_format_Init( &fmt, AUDIO_ES, VLC_FOURCC('X','A','J',0) );
    msg_Dbg( p_demux, "assuming EA ADPCM audio codec" );
    fmt.audio.i_rate = GetDWLE( &p_xa.nSamplesPerSec );
    fmt.audio.i_bytes_per_frame = 15 * GetWLE( &p_xa.nChannels );
    fmt.audio.i_frame_length = FRAME_LENGTH;
    fmt.audio.i_channels = GetWLE ( &p_xa.nChannels );
    fmt.audio.i_blockalign = fmt.audio.i_bytes_per_frame;
    fmt.audio.i_bitspersample = 16;
    fmt.i_bitrate = (fmt.audio.i_rate * fmt.audio.i_bytes_per_frame * 8)
                    / fmt.audio.i_frame_length;
    p_sys->i_data_offset = stream_Tell( p_demux->s );
    /* FIXME: better computation */
    p_sys->i_data_size = p_xa.iSize * 15 / 56;
    /* How many frames per block (1:1 is too CPU intensive) */
    p_sys->i_block_frames = fmt.audio.i_rate / (FRAME_LENGTH * 20) + 1;
    p_sys->i_frame_size = fmt.audio.i_bytes_per_frame;
    p_sys->i_bitrate = fmt.i_bitrate;
    msg_Dbg( p_demux, "fourcc: %4.4s, channels: %d, "
             "freq: %d Hz, bitrate: %dKo/s, blockalign: %d",
             (char *)&fmt.i_codec, fmt.audio.i_channels, fmt.audio.i_rate,
             fmt.i_bitrate / 8192, fmt.audio.i_blockalign );
    if( fmt.audio.i_rate == 0 || fmt.audio.i_channels == 0 )
    {
        free( p_sys );
        return VLC_EGENERIC;
    }
    p_sys->p_es = es_out_Add( p_demux->out, &fmt );
    date_Init( &p_sys->pts, fmt.audio.i_rate, 1 );
    date_Set( &p_sys->pts, VLC_TS_0 );
    return VLC_SUCCESS;
}
