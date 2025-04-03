 *****************************************************************************/
static subpicture_t *Decode( decoder_t *p_dec, block_t **pp_block )
{
    block_t       *p_block;
    subpicture_t  *p_spu = NULL;
    if( ( pp_block == NULL ) || ( *pp_block == NULL ) ) return NULL;
    p_block = *pp_block;
    *pp_block = NULL;
    if( ( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) ) ||
          p_block->i_buffer < sizeof(uint16_t) )
    {
        block_Release( p_block );
        return NULL;
    }
    uint8_t *p_buf = p_block->p_buffer;
    /* Read our raw string and create the styled segment for HTML */
    uint16_t i_psz_bytelength = GetWBE( p_buf );
    const uint8_t *p_pszstart = p_block->p_buffer + sizeof(uint16_t);
    char *psz_subtitle;
    if ( i_psz_bytelength > 2 &&
         ( !memcmp( p_pszstart, "\xFE\xFF", 2 ) || !memcmp( p_pszstart, "\xFF\xFE", 2 ) )
       )
    {
        psz_subtitle = FromCharset( "UTF-16", p_pszstart, i_psz_bytelength );
        if ( !psz_subtitle ) return NULL;
    }
    else
    {
        psz_subtitle = malloc( i_psz_bytelength + 1 );
        if ( !psz_subtitle ) return NULL;
        memcpy( psz_subtitle, p_pszstart, i_psz_bytelength );
        psz_subtitle[ i_psz_bytelength ] = '\0';
    }
    p_buf += i_psz_bytelength + sizeof(uint16_t);
    for( uint16_t i=0; i < i_psz_bytelength; i++ )
     if ( psz_subtitle[i] == '\r' ) psz_subtitle[i] = '\n';
    segment_t *p_segment = calloc( 1, sizeof(segment_t) );
    if ( !p_segment )
    {
        free( psz_subtitle );
        return NULL;
    }
    p_segment->psz_string = strdup( psz_subtitle );
    p_segment->i_size = str8len( psz_subtitle );
    if ( p_dec->fmt_in.subs.p_style )
    {
        p_segment->styles.i_color = p_dec->fmt_in.subs.p_style->i_font_color;
        p_segment->styles.i_color |= p_dec->fmt_in.subs.p_style->i_font_alpha << 24;
        if ( p_dec->fmt_in.subs.p_style->i_style_flags )
            p_segment->styles.i_flags = p_dec->fmt_in.subs.p_style->i_style_flags;
        p_segment->styles.i_fontsize = p_dec->fmt_in.subs.p_style->i_font_size;
    }
    if ( !p_segment->psz_string )
    {
        SegmentFree( p_segment );
        free( psz_subtitle );
        return NULL;
    }
    /* Create the subpicture unit */
    p_spu = decoder_NewSubpictureText( p_dec );
    if( !p_spu )
    {
        free( psz_subtitle );
        SegmentFree( p_segment );
        return NULL;
    }
    subpicture_updater_sys_t *p_spu_sys = p_spu->updater.p_sys;
    /* Parse our styles */
    while( (size_t)(p_buf - p_block->p_buffer) + 8 < p_block->i_buffer )
    {
        uint32_t i_atomsize = GetDWBE( p_buf );
        vlc_fourcc_t i_atomtype = VLC_FOURCC(p_buf[4],p_buf[5],p_buf[6],p_buf[7]);
        p_buf += 8;
        switch( i_atomtype )
        {
        case VLC_FOURCC('s','t','y','l'):
        {
            if ( (size_t)(p_buf - p_block->p_buffer) < 14 ) break;
            uint16_t i_nbrecords = GetWBE(p_buf);
            uint16_t i_cur_record = 0;
            p_buf += 2;
            while( i_cur_record++ < i_nbrecords )
            {
                if ( (size_t)(p_buf - p_block->p_buffer) < 12 ) break;
                uint16_t i_start = __MIN( GetWBE(p_buf), i_psz_bytelength - 1 );
                uint16_t i_end =  __MIN( GetWBE(p_buf + 2), i_psz_bytelength - 1 );
                segment_style_t style;
                style.i_flags = ConvertFlags( p_buf[6] );
                style.i_fontsize = p_buf[7];
                style.i_color = GetDWBE(p_buf+8) >> 8;// RGBA -> ARGB
                style.i_color |= (GetDWBE(p_buf+8) & 0xFF) << 24;
                ApplySegmentStyle( &p_segment, i_start, i_end, &style );
                if ( i_nbrecords == 1 )
                {
                    if ( p_buf[6] )
                    {
                        p_spu_sys->style_flags.i_value = ConvertFlags( p_buf[6] );
                        p_spu_sys->style_flags.b_set = true;
                    }
                    p_spu_sys->i_font_height_abs_to_src = p_buf[7];
                    p_spu_sys->font_color.i_value = GetDWBE(p_buf+8) >> 8;// RGBA -> ARGB
                    p_spu_sys->font_color.i_value |= (GetDWBE(p_buf+8) & 0xFF) << 24;
                    p_spu_sys->font_color.b_set = true;
                }
                p_buf += 12;
            }
        }   break;
        case VLC_FOURCC('d','r','p','o'):
            if ( (size_t)(p_buf - p_block->p_buffer) < 4 ) break;
            p_spu_sys->i_drop_shadow = __MAX( GetWBE(p_buf), GetWBE(p_buf+2) );
            break;
        case VLC_FOURCC('d','r','p','t'):
            if ( (size_t)(p_buf - p_block->p_buffer) < 2 ) break;
            p_spu_sys->i_drop_shadow_alpha = GetWBE(p_buf);
            break;
        default:
            break;
        }
        p_buf += i_atomsize;
    }
    p_spu->i_start    = p_block->i_pts;
    p_spu->i_stop     = p_block->i_pts + p_block->i_length;
    p_spu->b_ephemer  = (p_block->i_length == 0);
    p_spu->b_absolute = false;
    p_spu_sys->align = SUBPICTURE_ALIGN_BOTTOM;
    p_spu_sys->text  = psz_subtitle;
    p_spu_sys->p_htmlsegments = p_segment;
    block_Release( p_block );
    return p_spu;
}
