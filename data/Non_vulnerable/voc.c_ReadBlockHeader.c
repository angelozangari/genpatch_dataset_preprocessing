}
static int ReadBlockHeader( demux_t *p_demux )
{
    es_format_t     new_fmt;
    uint8_t buf[8];
    int32_t i_block_size;
    demux_sys_t *p_sys = p_demux->p_sys;
    if( stream_Read( p_demux->s, buf, 4 ) < 4 )
        return VLC_EGENERIC; /* EOF */
    i_block_size = GetDWLE( buf ) >> 8;
    msg_Dbg( p_demux, "new block: type: %u, size: %u",
             (unsigned)*buf, i_block_size );
    es_format_Init( &new_fmt, AUDIO_ES, 0 );
    switch( *buf )
    {
        case 0: /* not possible : caught with earlier stream_Read */
            goto corrupt;
        case 1:
            if( i_block_size < 2 )
                goto corrupt;
            i_block_size -= 2;
            if( stream_Read( p_demux->s, buf, 2 ) < 2 )
                goto corrupt;
            if( buf[1] )
            {
                msg_Err( p_demux, "unsupported compression" );
                return VLC_EGENERIC;
            }
            new_fmt.i_codec = VLC_CODEC_U8;
            new_fmt.audio.i_rate = fix_voc_sr( 1000000L / (256L - buf[0]) );
            new_fmt.audio.i_bytes_per_frame = 1;
            new_fmt.audio.i_frame_length = 1;
            new_fmt.audio.i_channels = 1;
            new_fmt.audio.i_blockalign = 1;
            new_fmt.audio.i_bitspersample = 8;
            new_fmt.i_bitrate = new_fmt.audio.i_rate * 8;
            break;
        case 2: /* data block with same format as the previous one */
            if( p_sys->p_es == NULL )
                goto corrupt; /* no previous block! */
            memcpy( &new_fmt, &p_sys->fmt, sizeof( new_fmt ) );
            break;
        case 3: /* silence block */
            if( ( i_block_size != 3 )
             || ( stream_Read( p_demux->s, buf, 3 ) < 3 ) )
                goto corrupt;
            i_block_size = 0;
            p_sys->i_silence_countdown = GetWLE( buf );
            new_fmt.i_codec = VLC_CODEC_U8;
            new_fmt.audio.i_rate = fix_voc_sr( 1000000L / (256L - buf[0]) );
            new_fmt.audio.i_bytes_per_frame = 1;
            new_fmt.audio.i_frame_length = 1;
            new_fmt.audio.i_channels = 1;
            new_fmt.audio.i_blockalign = 1;
            new_fmt.audio.i_bitspersample = 8;
            new_fmt.i_bitrate = new_fmt.audio.i_rate * 8;
            break;
        case 6: /* repeat block */
            if( ( i_block_size != 2 )
             || ( stream_Read( p_demux->s, buf, 2 ) < 2 ) )
                goto corrupt;
            i_block_size = 0;
            p_sys->i_loop_count = GetWLE( buf );
            p_sys->i_loop_offset = stream_Tell( p_demux->s );
            break;
        case 7: /* repeat end block */
            if( i_block_size != 0 )
                goto corrupt;
            if( p_sys->i_loop_count > 0 )
            {
                if( stream_Seek( p_demux->s, p_sys->i_loop_offset ) )
                    msg_Warn( p_demux, "cannot loop: seek failed" );
                else
                    p_sys->i_loop_count--;
            }
            break;
        case 8:
            /*
             * Block 8 is a big kludge to add stereo support to block 1 :
             * A block of type 8 is always followed by a block of type 1
             * and specifies the number of channels in that 1-block
             * (normally block 1 are always mono). In practice, block type 9
             * is used for stereo rather than 8
             */
            if( ( i_block_size != 4 )
             || ( stream_Read( p_demux->s, buf, 4 ) < 4 ) )
                goto corrupt;
            if( buf[2] )
            {
                msg_Err( p_demux, "unsupported compression" );
                return VLC_EGENERIC;
            }
            new_fmt.i_codec = VLC_CODEC_U8;
            if (buf[3] >= 32)
                goto corrupt;
            new_fmt.audio.i_channels = buf[3] + 1; /* can't be nul */
            new_fmt.audio.i_rate = 256000000L /
                          ((65536L - GetWLE(buf)) * new_fmt.audio.i_channels);
            new_fmt.audio.i_bytes_per_frame = new_fmt.audio.i_channels;
            new_fmt.audio.i_frame_length = 1;
            new_fmt.audio.i_blockalign = new_fmt.audio.i_bytes_per_frame;
            new_fmt.audio.i_bitspersample = 8 * new_fmt.audio.i_bytes_per_frame;
            new_fmt.i_bitrate = new_fmt.audio.i_rate * 8;
            /* read subsequent block 1 */
            if( stream_Read( p_demux->s, buf, 4 ) < 4 )
                return VLC_EGENERIC; /* EOF */
            i_block_size = GetDWLE( buf ) >> 8;
            msg_Dbg( p_demux, "new block: type: %u, size: %u",
                    (unsigned)*buf, i_block_size );
            if( i_block_size < 2 )
                goto corrupt;
            i_block_size -= 2;
            if( stream_Read( p_demux->s, buf, 2 ) < 2 )
                goto corrupt;
            if( buf[1] )
            {
                msg_Err( p_demux, "unsupported compression" );
                return VLC_EGENERIC;
            }
            break;
        case 9: /* newer data block with channel number and bits resolution */
            if( i_block_size < 12 )
                goto corrupt;
            i_block_size -= 12;
            if( ( stream_Read( p_demux->s, buf, 8 ) < 8 )
             || ( stream_Read( p_demux->s, NULL, 4 ) < 4 ) )
                goto corrupt;
            new_fmt.audio.i_rate = GetDWLE( buf );
            new_fmt.audio.i_bitspersample = buf[4];
            new_fmt.audio.i_channels = buf[5];
            switch( GetWLE( &buf[6] ) ) /* format */
            {
                case 0x0000: /* PCM */
                    switch( new_fmt.audio.i_bitspersample )
                    {
                        case 8:
                            new_fmt.i_codec = VLC_CODEC_U8;
                            break;
                        case 16:
                            new_fmt.i_codec = VLC_CODEC_U16L;
                            break;
                        default:
                            msg_Err( p_demux, "unsupported bit res.: %u bits",
                                     new_fmt.audio.i_bitspersample );
                            return VLC_EGENERIC;
                    }
                    break;
                case 0x0004: /* signed */
                    switch( new_fmt.audio.i_bitspersample )
                    {
                        case 8:
                            new_fmt.i_codec = VLC_CODEC_S8;
                            break;
                        case 16:
                            new_fmt.i_codec = VLC_CODEC_S16L;
                            break;
                        default:
                            msg_Err( p_demux, "unsupported bit res.: %u bits",
                                     new_fmt.audio.i_bitspersample );
                            return VLC_EGENERIC;
                    }
                    break;
                default:
                    msg_Err( p_demux, "unsupported compression" );
                    return VLC_EGENERIC;
            }
            if( new_fmt.audio.i_channels == 0 )
            {
                msg_Err( p_demux, "0 channels detected" );
                return VLC_EGENERIC;
            }
            new_fmt.audio.i_bytes_per_frame = new_fmt.audio.i_channels
                * (new_fmt.audio.i_bitspersample / 8);
            new_fmt.audio.i_frame_length = 1;
            new_fmt.audio.i_blockalign = new_fmt.audio.i_bytes_per_frame;
            new_fmt.i_bitrate = 8 * new_fmt.audio.i_rate
                                     * new_fmt.audio.i_bytes_per_frame;
            break;
        default:
            msg_Dbg( p_demux, "unknown block type %u - skipping block",
                     (unsigned)*buf);
        case 4: /* blocks of non-audio types can be skipped */
        case 5:
            if( stream_Read( p_demux->s, NULL, i_block_size ) < i_block_size )
                goto corrupt;
            i_block_size = 0;
            break;
    }
    p_sys->i_block_start = stream_Tell( p_demux->s );
    p_sys->i_block_end = p_sys->i_block_start + i_block_size;
    if( i_block_size || p_sys->i_silence_countdown )
    {
        /* we've read a block with data in it - update decoder */
        msg_Dbg( p_demux, "fourcc: %4.4s, channels: %d, "
                 "freq: %d Hz, bitrate: %dKo/s, blockalign: %d, "
                 "bits/samples: %d", (char *)&new_fmt.i_codec,
                 new_fmt.audio.i_channels, new_fmt.audio.i_rate,
                 new_fmt.i_bitrate / 8192, new_fmt.audio.i_blockalign,
                 new_fmt.audio.i_bitspersample );
        if( ( p_sys->p_es != NULL ) && fmtcmp( &p_sys->fmt, &new_fmt ) )
        {
            msg_Dbg( p_demux, "codec change needed" );
            es_out_Del( p_demux->out, p_sys->p_es );
            p_sys->p_es = NULL;
        }
        if( p_sys->p_es == NULL )
        {
            memcpy( &p_sys->fmt, &new_fmt, sizeof( p_sys->fmt ) );
            date_Change( &p_sys->pts, p_sys->fmt.audio.i_rate, 1 );
            p_sys->p_es = es_out_Add( p_demux->out, &p_sys->fmt );
        }
    }
    return VLC_SUCCESS;
corrupt:
    msg_Err( p_demux, "corrupted file - halting demux" );
    return VLC_EGENERIC;
}
