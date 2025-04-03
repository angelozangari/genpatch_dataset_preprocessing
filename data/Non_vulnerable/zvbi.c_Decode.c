 *****************************************************************************/
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t   *p_sys = p_dec->p_sys;
    block_t         *p_block;
    subpicture_t    *p_spu = NULL;
    video_format_t  fmt;
    bool            b_cached = false;
    vbi_page        p_page;
    if( (pp_block == NULL) || (*pp_block == NULL) )
        return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    if( p_block->i_buffer > 0 &&
        ( ( p_block->p_buffer[0] >= 0x10 && p_block->p_buffer[0] <= 0x1f ) ||
          ( p_block->p_buffer[0] >= 0x99 && p_block->p_buffer[0] <= 0x9b ) ) )
    {
        vbi_sliced   *p_sliced = p_sys->p_vbi_sliced;
        unsigned int i_lines = 0;
        p_block->i_buffer--;
        p_block->p_buffer++;
        while( p_block->i_buffer >= 2 )
        {
            int      i_id   = p_block->p_buffer[0];
            unsigned i_size = p_block->p_buffer[1];
            if( 2 + i_size > p_block->i_buffer )
                break;
            if( ( i_id == 0x02 || i_id == 0x03 ) && i_size >= 44 && i_lines < MAX_SLICES )
            {
                if(p_block->p_buffer[3] == 0xE4 )    /* framing_code */
                {
                    unsigned line_offset  = p_block->p_buffer[2] & 0x1f;
                    unsigned field_parity = p_block->p_buffer[2] & 0x20;
                    p_sliced[i_lines].id = VBI_SLICED_TELETEXT_B;
                    if( line_offset > 0 )
                        p_sliced[i_lines].line = line_offset + (field_parity ? 0 : 313);
                    else
                        p_sliced[i_lines].line = 0;
                    for( int i = 0; i < 42; i++ )
                        p_sliced[i_lines].data[i] = vbi_rev8( p_block->p_buffer[4 + i] );
                    i_lines++;
                }
            }
            p_block->i_buffer -= 2 + i_size;
            p_block->p_buffer += 2 + i_size;
        }
        if( i_lines > 0 )
            vbi_decode( p_sys->p_vbi_dec, p_sliced, i_lines, 0 );
    }
    /* */
    vlc_mutex_lock( &p_sys->lock );
    const int i_align = p_sys->i_align;
    const unsigned int i_wanted_page = p_sys->i_wanted_page;
    const unsigned int i_wanted_subpage = p_sys->i_wanted_subpage;
    const bool b_opaque = p_sys->b_opaque;
    vlc_mutex_unlock( &p_sys->lock );
    /* Try to see if the page we want is in the cache yet */
    memset( &p_page, 0, sizeof(vbi_page) );
    b_cached = vbi_fetch_vt_page( p_sys->p_vbi_dec, &p_page,
                                  vbi_dec2bcd( i_wanted_page ),
                                  i_wanted_subpage, VBI_WST_LEVEL_3p5,
                                  25, true );
    if( i_wanted_page == p_sys->i_last_page && !p_sys->b_update )
        goto error;
    if( !b_cached )
    {
        if( p_sys->b_text && p_sys->i_last_page != i_wanted_page )
        {
            /* We need to reset the subtitle */
            p_spu = Subpicture( p_dec, &fmt, true,
                                p_page.columns, p_page.rows,
                                i_align, p_block->i_pts );
            if( !p_spu )
                goto error;
            subpicture_updater_sys_t *p_spu_sys = p_spu->updater.p_sys;
            p_spu_sys->text = strdup("");
            p_sys->b_update = true;
            p_sys->i_last_page = i_wanted_page;
            goto exit;
        }
        goto error;
    }
    p_sys->b_update = false;
    p_sys->i_last_page = i_wanted_page;
#ifdef ZVBI_DEBUG
    msg_Dbg( p_dec, "we now have page: %d ready for display",
             i_wanted_page );
#endif
    /* Ignore transparent rows at the beginning and end */
    int i_first_row = get_first_visible_row( p_page.text, p_page.rows, p_page.columns );
    int i_num_rows;
    if ( i_first_row < 0 ) {
        i_first_row = p_page.rows - 1;
        i_num_rows = 0;
    } else {
        i_num_rows = get_last_visible_row( p_page.text, p_page.rows, p_page.columns ) - i_first_row + 1;
    }
#ifdef ZVBI_DEBUG
    msg_Dbg( p_dec, "After top and tail of page we have rows %i-%i of %i",
             i_first_row + 1, i_first_row + i_num_rows, p_page.rows );
#endif
    /* If there is a page or sub to render, then we do that here */
    /* Create the subpicture unit */
    p_spu = Subpicture( p_dec, &fmt, p_sys->b_text,
                        p_page.columns, i_num_rows,
                        i_align, p_block->i_pts );
    if( !p_spu )
        goto error;
    if( p_sys->b_text )
    {
        unsigned int i_textsize = 7000;
        int i_total,offset;
        char p_text[i_textsize+1];
        i_total = vbi_print_page_region( &p_page, p_text, i_textsize,
                        "UTF-8", 0, 0, 0, i_first_row, p_page.columns, i_num_rows );
        for( offset=1; offset<i_total && isspace( p_text[i_total-offset ] ); offset++)
           p_text[i_total-offset] = '\0';
        i_total -= offset;
        offset=0;
        while( offset < i_total && isspace( p_text[offset] ) )
           offset++;
        subpicture_updater_sys_t *p_spu_sys = p_spu->updater.p_sys;
        p_spu_sys->text = strdup( &p_text[offset] );
        p_spu_sys->align = i_align;
        p_spu_sys->i_font_height_percent = 5;
        p_spu_sys->renderbg = b_opaque;
#ifdef ZVBI_DEBUG
        msg_Info( p_dec, "page %x-%x(%d)\n\"%s\"", p_page.pgno, p_page.subno, i_total, &p_text[offset] );
#endif
    }
    else
    {
        picture_t *p_pic = p_spu->p_region->p_picture;
        /* ZVBI is stupid enough to assume pitch == width */
        p_pic->p->i_pitch = 4 * fmt.i_width;
        /* Maintain subtitle postion */
        p_spu->p_region->i_y = i_first_row*10;
        p_spu->i_original_picture_width = p_page.columns*12;
        p_spu->i_original_picture_height = p_page.rows*10;
        vbi_draw_vt_page_region( &p_page, ZVBI_PIXFMT_RGBA32,
                          p_spu->p_region->p_picture->p->p_pixels, -1,
                          0, i_first_row, p_page.columns, i_num_rows,
                          1, 1);
        vlc_mutex_lock( &p_sys->lock );
        memcpy( p_sys->nav_link, &p_page.nav_link, sizeof( p_sys->nav_link )) ;
        vlc_mutex_unlock( &p_sys->lock );
        OpaquePage( p_pic, &p_page, fmt, b_opaque, i_first_row * p_page.columns );
    }
exit:
    vbi_unref_page( &p_page );
    block_Release( p_block );
    return p_spu;
error:
    vbi_unref_page( &p_page );
    if( p_spu != NULL )
    {
        decoder_DeleteSubpicture( p_dec, p_spu );
        p_spu = NULL;
    }
    block_Release( p_block );
    return NULL;
}
