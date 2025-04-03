 *****************************************************************************/
static int Open( vlc_object_t * p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    const uint8_t *p_buf;
    uint16_t    i_data_offset, i_version;
    if( stream_Peek( p_demux->s, &p_buf, 26 ) < 26 )
        return VLC_EGENERIC;
    if( memcmp( p_buf, ct_header, 20 ) )
        return VLC_EGENERIC;
    p_buf += 20;
    i_data_offset = GetWLE( p_buf );
    if ( i_data_offset < 26 /* not enough room for full VOC header */ )
        return VLC_EGENERIC;
    p_buf += 2;
    i_version = GetWLE( p_buf );
    if( ( i_version != 0x10A ) && ( i_version != 0x114 ) )
        return VLC_EGENERIC; /* unknown VOC version */
    p_buf += 2;
    if( GetWLE( p_buf ) != (uint16_t)(0x1234 + ~i_version) )
        return VLC_EGENERIC;
    /* We have a valid VOC header */
    msg_Dbg( p_demux, "CT Voice file v%d.%d", i_version >> 8,
             i_version & 0xff );
    /* skip VOC header */
    if( stream_Read( p_demux->s, NULL, i_data_offset ) < i_data_offset )
        return VLC_EGENERIC;
    p_demux->pf_demux   = Demux;
    p_demux->pf_control = Control;
    p_demux->p_sys      = p_sys = malloc( sizeof( demux_sys_t ) );
    if( p_sys == NULL )
        return VLC_ENOMEM;
    p_sys->i_silence_countdown = p_sys->i_block_start = p_sys->i_block_end =
    p_sys->i_loop_count = 0;
    p_sys->p_es = NULL;
    date_Init( &p_sys->pts, 1, 1 );
    date_Set( &p_sys->pts, 1 );
    es_format_Init( &p_sys->fmt, AUDIO_ES, 0 );
    return VLC_SUCCESS;
}
