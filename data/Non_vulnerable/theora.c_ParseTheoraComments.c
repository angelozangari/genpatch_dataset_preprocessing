 *****************************************************************************/
static void ParseTheoraComments( decoder_t *p_dec )
{
    char *psz_name, *psz_value, *psz_comment;
    int i = 0;
    /* Regarding the th_comment structure: */
    /* The metadata is stored as a series of (tag, value) pairs, in
       length-encoded string vectors. The first occurrence of the '='
       character delimits the tag and value. A particular tag may
       occur more than once, and order is significant. The character
       set encoding for the strings is always UTF-8, but the tag names
       are limited to ASCII, and treated as case-insensitive. See the
       Theora specification, Section 6.3.3 for details. */
    /* In filling in this structure, th_decode_headerin() will
       null-terminate the user_comment strings for safety. However,
       the bitstream format itself treats them as 8-bit clean vectors,
       possibly containing null characters, and so the length array
       should be treated as their authoritative length. */
    while ( i < p_dec->p_sys->tc.comments )
    {
        int clen = p_dec->p_sys->tc.comment_lengths[i];
        if ( clen <= 0 || clen >= INT_MAX ) { i++; continue; }
        psz_comment = (char *)malloc( clen + 1 );
        if( !psz_comment )
            break;
        memcpy( (void*)psz_comment, (void*)p_dec->p_sys->tc.user_comments[i], clen + 1 );
        psz_name = psz_comment;
        psz_value = strchr( psz_comment, '=' );
        if( psz_value )
        {
            *psz_value = '\0';
            psz_value++;
            if( !p_dec->p_description )
                p_dec->p_description = vlc_meta_New();
            /* TODO:  Since psz_value can contain NULLs see if there is an
             * instance where we need to preserve the full length of this string */
            if( p_dec->p_description )
                vlc_meta_AddExtra( p_dec->p_description, psz_name, psz_value );
        }
        free( psz_comment );
        i++;
    }
}
