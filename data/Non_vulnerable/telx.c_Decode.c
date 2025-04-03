 *****************************************************************************/
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t       *p_block;
    subpicture_t  *p_spu = NULL;
    video_format_t fmt;
    /* int erase = 0; */
    int len, offset;
#if 0
    int i_wanted_magazine = i_conf_wanted_page / 100;
    int i_wanted_page = 0x10 * ((i_conf_wanted_page % 100) / 10)
                         | (i_conf_wanted_page % 10);
#endif
    bool b_update = false;
    char psz_text[512], *pt = psz_text;
    char psz_line[256];
    int i, total;
    if( pp_block == NULL || *pp_block == NULL )
        return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    dbg((p_dec, "start of telx packet with header %2x\n",
                * (uint8_t *) p_block->p_buffer));
    len = p_block->i_buffer;
    for ( offset = 1; offset + 46 <= len; offset += 46 )
    {
        uint8_t * packet = (uint8_t *) p_block->p_buffer+offset;
//        int vbi = ((0x20 & packet[2]) != 0 ? 0 : 313) + (0x1F & packet[2]);
//        dbg((p_dec, "vbi %d header %02x %02x %02x\n", vbi, packet[0], packet[1], packet[2]));
        if ( packet[0] == 0xFF ) continue;
/*      if (packet[1] != 0x2C) { */
/*         printf("wrong header\n"); */
/*         //goto error; */
/*         continue; */
/*       } */
        int mpag = (hamming_8_4( packet[4] ) << 4) | hamming_8_4( packet[5] );
        int row, magazine;
        if ( mpag < 0 )
        {
            /* decode error */
            dbg((p_dec, "mpag hamming error\n"));
            continue;
        }
        row = 0xFF & bytereverse(mpag);
        magazine = (7 & row) == 0 ? 8 : (7 & row);
        row >>= 3;
        if ( p_sys->i_wanted_page != -1
              && magazine != p_sys->i_wanted_magazine )
            continue;
        if ( row == 0 )
        {
            /* row 0 : flags and header line */
            int flag = 0;
            int a;
            for ( a = 0; a < 6; a++ )
            {
                flag |= (0xF & (bytereverse( hamming_8_4(packet[8 + a]) ) >> 4))
                          << (a * 4);
            }
    /*         if (!p_sys->b_ignore_sub_flag && !(1 & flag>>15)) */
    /*           continue; */
            p_sys->i_page[magazine] = (0xF0 & bytereverse( hamming_8_4(packet[7]) )) |
                             (0xF & (bytereverse( hamming_8_4(packet[6]) ) >> 4) );
            decode_string( psz_line, sizeof(psz_line), p_sys, magazine,
                           packet + 14, 40 - 14 );
            dbg((p_dec, "mag %d flags %x page %x character set %d subtitles %d", magazine, flag,
                 p_sys->i_page[magazine],
                 7 & flag>>21, 1 & flag>>15, psz_line));
            p_sys->pi_active_national_set[magazine] =
                                 ppi_national_subsets[7 & (flag >> 21)];
            p_sys->b_is_subtitle[magazine] = p_sys->b_ignore_sub_flag
                                              || ( (1 & (flag >> 15))
                                                  && (1 & (flag>>16)) );
            dbg(( p_dec, "FLAGS%s%s%s%s%s%s%s mag_ser %d",
                  (1 & (flag>>14))? " news" : "",
                  (1 & (flag>>15))? " subtitle" : "",
                  (1 & (flag>>7))? " erase" : "",
                  (1 & (flag>>16))? " suppressed_head" : "",
                  (1 & (flag>>17))? " update" : "",
                  (1 & (flag>>18))? " interrupt" : "",
                  (1 & (flag>>19))? " inhibit" : "",
                  (1 & (flag>>20)) ));
            if ( (p_sys->i_wanted_page != -1
                   && p_sys->i_page[magazine] != p_sys->i_wanted_page)
                   || !p_sys->b_is_subtitle[magazine] )
                continue;
            p_sys->b_erase[magazine] = (1 & (flag >> 7));
            dbg((p_dec, "%ld --> %ld\n", (long int) p_block->i_pts, (long int)(p_sys->prev_pts+1500000)));
            /* kludge here :
             * we ignore the erase flag if it happens less than 1.5 seconds
             * before last caption
             * TODO   make this time configurable
             * UPDATE the kludge seems to be no more necessary
             *        so it's commented out*/
            if ( /*p_block->i_pts > p_sys->prev_pts + 1500000 && */
                 p_sys->b_erase[magazine] )
            {
                int i;
                dbg((p_dec, "ERASE !\n"));
                p_sys->b_erase[magazine] = 0;
                for ( i = 1; i < 32; i++ )
                {
                    if ( !p_sys->ppsz_lines[i][0] ) continue;
                    /* b_update = true; */
                    p_sys->ppsz_lines[i][0] = 0;
                }
            }
            /* replace the row if it's different */
            if ( strcmp(psz_line, p_sys->ppsz_lines[row]) )
            {
                strncpy( p_sys->ppsz_lines[row], psz_line,
                         sizeof(p_sys->ppsz_lines[row]) - 1);
            }
            b_update = true;
        }
        else if ( row < 24 )
        {
            char * t;
            int i;
            /* row 1-23 : normal lines */
            if ( (p_sys->i_wanted_page != -1
                   && p_sys->i_page[magazine] != p_sys->i_wanted_page)
                   || !p_sys->b_is_subtitle[magazine]
                   || (p_sys->i_wanted_page == -1
                        && p_sys->i_page[magazine] > 0x99) )
                continue;
            decode_string( psz_line, sizeof(psz_line), p_sys, magazine,
                           packet + 6, 40 );
            t = psz_line;
            /* remove starting spaces */
            while ( *t == 32 ) t++;
            /* remove trailing spaces */
            for ( i = strlen(t) - 1; i >= 0 && t[i] == 32; i-- );
            t[i + 1] = 0;
            /* replace the row if it's different */
            if ( strcmp( t, p_sys->ppsz_lines[row] ) )
            {
                strncpy( p_sys->ppsz_lines[row], t,
                         sizeof(p_sys->ppsz_lines[row]) - 1 );
                b_update = true;
            }
            if (t[0])
                p_sys->prev_pts = p_block->i_pts;
            dbg((p_dec, "%d %d : ", magazine, row));
            dbg((p_dec, "%s\n", t));
#ifdef TELX_DEBUG
            {
                char dbg[256];
                dbg[0] = 0;
                for ( i = 0; i < 40; i++ )
                {
                    int in = bytereverse(packet[6 + i]) & 0x7f;
                    sprintf(dbg + strlen(dbg), "%02x ", in);
                }
                dbg((p_dec, "%s\n", dbg));
                dbg[0] = 0;
                for ( i = 0; i < 40; i++ )
                {
                    decode_string( psz_line, sizeof(psz_line), p_sys, magazine,
                                   packet + 6 + i, 1 );
                    sprintf( dbg + strlen(dbg), "%s  ", psz_line );
                }
                dbg((p_dec, "%s\n", dbg));
            }
#endif
        }
        else if ( row == 25 )
        {
            /* row 25 : alternate header line */
            if ( (p_sys->i_wanted_page != -1
                   && p_sys->i_page[magazine] != p_sys->i_wanted_page)
                   || !p_sys->b_is_subtitle[magazine] )
                continue;
            decode_string( psz_line, sizeof(psz_line), p_sys, magazine,
                           packet + 6, 40 );
            /* replace the row if it's different */
            if ( strcmp( psz_line, p_sys->ppsz_lines[0] ) )
            {
                strncpy( p_sys->ppsz_lines[0], psz_line,
                         sizeof(p_sys->ppsz_lines[0]) - 1 );
                /* b_update = true; */
            }
        }
/*       else if (row == 26) { */
/*         // row 26 : TV listings */
/*       } else */
/*         dbg((p_dec, "%d %d : %s\n", magazine, row, decode_string(p_sys, magazine, packet+6, 40))); */
    }
    if ( !b_update )
        goto error;
    total = 0;
    for ( i = 1; i < 24; i++ )
    {
        size_t l = strlen( p_sys->ppsz_lines[i] );
        if ( l > sizeof(psz_text) - total - 1 )
            l = sizeof(psz_text) - total - 1;
        if ( l > 0 )
        {
            memcpy( pt, p_sys->ppsz_lines[i], l );
            total += l;
            pt += l;
            if ( sizeof(psz_text) - total - 1 > 0 )
            {
                *pt++ = '\n';
                total++;
            }
        }
    }
    *pt = 0;
    if ( !strcmp(psz_text, p_sys->psz_prev_text) )
        goto error;
    dbg((p_dec, "UPDATE TELETEXT PICTURE\n"));
    assert( sizeof(p_sys->psz_prev_text) >= sizeof(psz_text) );
    strcpy( p_sys->psz_prev_text, psz_text );
    /* Create the subpicture unit */
    p_spu = decoder_NewSubpicture( p_dec, NULL );
    if( !p_spu )
    {
        msg_Warn( p_dec, "can't get spu buffer" );
        goto error;
    }
    /* Create a new subpicture region */
    memset( &fmt, 0, sizeof(video_format_t) );
    fmt.i_chroma = VLC_CODEC_TEXT;
    fmt.i_width = fmt.i_height = 0;
    fmt.i_x_offset = fmt.i_y_offset = 0;
    p_spu->p_region = subpicture_region_New( &fmt );
    if( p_spu->p_region == NULL )
    {
        msg_Err( p_dec, "cannot allocate SPU region" );
        goto error;
    }
    /* Normal text subs, easy markup */
    p_spu->p_region->i_align = SUBPICTURE_ALIGN_BOTTOM | p_sys->i_align;
    p_spu->p_region->i_x = p_sys->i_align ? 20 : 0;
    p_spu->p_region->i_y = 10;
    p_spu->p_region->psz_text = strdup(psz_text);
    p_spu->i_start = p_block->i_pts;
    p_spu->i_stop = p_block->i_pts + p_block->i_length;
    p_spu->b_ephemer = (p_block->i_length == 0);
    p_spu->b_absolute = false;
    dbg((p_dec, "%ld --> %ld\n", (long int) p_block->i_pts/100000, (long int)p_block->i_length/100000));
    block_Release( p_block );
    return p_spu;
error:
    if ( p_spu != NULL )
    {
        decoder_DeleteSubpicture( p_dec, p_spu );
        p_spu = NULL;
    }
    block_Release( p_block );
    return NULL;
}
