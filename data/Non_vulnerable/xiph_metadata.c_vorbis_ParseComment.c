}
void vorbis_ParseComment( es_format_t *p_fmt, vlc_meta_t **pp_meta,
        const uint8_t *p_data, size_t i_data,
        int *i_attachments, input_attachment_t ***attachments,
        int *i_cover_score, int *i_cover_idx,
        int *i_seekpoint, seekpoint_t ***ppp_seekpoint,
        float (* ppf_replay_gain)[AUDIO_REPLAY_GAIN_MAX],
        float (* ppf_replay_peak)[AUDIO_REPLAY_GAIN_MAX] )
{
    if( i_data < 8 )
        return;
    uint32_t vendor_length = GetDWLE(p_data); RM(4);
    if( vendor_length > i_data )
        return; /* invalid length */
    RM(vendor_length); /* TODO: handle vendor payload */
    if( i_data < 4 )
        return;
    uint32_t i_comment = GetDWLE(p_data); RM(4);
    if( i_comment > i_data || i_comment == 0 )
        return; /* invalid length */
    /* */
    vlc_meta_t *p_meta = *pp_meta;
    if( !p_meta )
        *pp_meta = p_meta = vlc_meta_New();
    if( unlikely( !p_meta ) )
        return;
    /* */
    bool hasTitle        = false;
    bool hasArtist       = false;
    bool hasGenre        = false;
    bool hasCopyright    = false;
    bool hasAlbum        = false;
    bool hasTrackNum     = false;
    bool hasDescription  = false;
    bool hasRating       = false;
    bool hasDate         = false;
    bool hasLanguage     = false;
    bool hasPublisher    = false;
    bool hasEncodedBy    = false;
    bool hasTrackTotal   = false;
    chapters_array_t chapters_array = { 0, NULL };
    for( ; i_comment > 0 && i_data >= 4; i_comment-- )
    {
        uint32_t comment_size = GetDWLE(p_data); RM(4);
        if( comment_size > i_data )
            break;
        if( comment_size == 0 )
            continue;
        char* psz_comment = malloc( comment_size + 1 );
        if( unlikely( !psz_comment ) )
            goto next_comment;
        memcpy( psz_comment, p_data, comment_size );
        psz_comment[comment_size] = '\0';
        EnsureUTF8( psz_comment );
#define IF_EXTRACT(txt,var) \
    if( !strncasecmp(psz_comment, txt, strlen(txt)) ) \
    { \
        const char *oldval = vlc_meta_Get( p_meta, vlc_meta_ ## var ); \
        if( oldval && has##var) \
        { \
            char * newval; \
            if( asprintf( &newval, "%s,%s", oldval, &psz_comment[strlen(txt)] ) == -1 ) \
                newval = NULL; \
            vlc_meta_Set( p_meta, vlc_meta_ ## var, newval ); \
            free( newval ); \
        } \
        else \
            vlc_meta_Set( p_meta, vlc_meta_ ## var, &psz_comment[strlen(txt)] ); \
        has##var = true; \
    }
#define IF_EXTRACT_ONCE(txt,var) \
    if( !strncasecmp(psz_comment, txt, strlen(txt)) && !has##var ) \
    { \
        vlc_meta_Set( p_meta, vlc_meta_ ## var, &psz_comment[strlen(txt)] ); \
        has##var = true; \
    }
#define IF_EXTRACT_FMT(txt,var,fmt,target) \
    IF_EXTRACT(txt,var)\
    if( fmt && !strncasecmp(psz_comment, txt, strlen(txt)) )\
        {\
            if ( fmt->target ) free( fmt->target );\
            fmt->target = strdup(&psz_comment[strlen(txt)]);\
        }
        IF_EXTRACT("TITLE=", Title )
        else IF_EXTRACT("ARTIST=", Artist )
        else IF_EXTRACT("GENRE=", Genre )
        else IF_EXTRACT("COPYRIGHT=", Copyright )
        else IF_EXTRACT("ALBUM=", Album )
        else if( !hasTrackNum && !strncasecmp(psz_comment, "TRACKNUMBER=", strlen("TRACKNUMBER=" ) ) )
        {
            /* Yeah yeah, such a clever idea, let's put xx/xx inside TRACKNUMBER
             * Oh, and let's not use TRACKTOTAL or TOTALTRACKS... */
            short unsigned u_track, u_total;
            if( sscanf( &psz_comment[strlen("TRACKNUMBER=")], "%hu/%hu", &u_track, &u_total ) == 2 )
            {
                char str[6];
                snprintf(str, 6, "%u", u_track);
                vlc_meta_Set( p_meta, vlc_meta_TrackNumber, str );
                hasTrackNum = true;
                snprintf(str, 6, "%u", u_total);
                vlc_meta_Set( p_meta, vlc_meta_TrackTotal, str );
                hasTrackTotal = true;
            }
            else
            {
                vlc_meta_Set( p_meta, vlc_meta_TrackNumber, &psz_comment[strlen("TRACKNUMBER=")] );
                hasTrackNum = true;
            }
        }
        else IF_EXTRACT_ONCE("TRACKTOTAL=", TrackTotal )
        else IF_EXTRACT_ONCE("TOTALTRACKS=", TrackTotal )
        else IF_EXTRACT("DESCRIPTION=", Description )
        else IF_EXTRACT("COMMENT=", Description )
        else IF_EXTRACT("COMMENTS=", Description )
        else IF_EXTRACT("RATING=", Rating )
        else IF_EXTRACT("DATE=", Date )
        else IF_EXTRACT_FMT("LANGUAGE=", Language, p_fmt, psz_language )
        else IF_EXTRACT("ORGANIZATION=", Publisher )
        else IF_EXTRACT("ENCODER=", EncodedBy )
        else if( !strncasecmp( psz_comment, "METADATA_BLOCK_PICTURE=", strlen("METADATA_BLOCK_PICTURE=")))
        {
            if( attachments == NULL )
                goto next_comment;
            uint8_t *p_picture;
            size_t i_size = vlc_b64_decode_binary( &p_picture, &psz_comment[strlen("METADATA_BLOCK_PICTURE=")]);
            input_attachment_t *p_attachment = ParseFlacPicture( p_picture,
                i_size, *i_attachments, i_cover_score, i_cover_idx );
            free( p_picture );
            if( p_attachment )
            {
                TAB_APPEND_CAST( (input_attachment_t**),
                    *i_attachments, *attachments, p_attachment );
            }
        }
        else if ( ppf_replay_gain && ppf_replay_peak && !strncmp(psz_comment, "REPLAYGAIN_", 11) )
        {
            char *p = strchr( psz_comment, '=' );
            if (!p) goto next_comment;
            char *psz_val;
            if ( !strncasecmp(psz_comment, "REPLAYGAIN_TRACK_GAIN=", 22) )
            {
                psz_val = malloc( strlen(p+1) + 1 );
                if (!psz_val) continue;
                if( sscanf( ++p, "%s dB", psz_val ) == 1 )
                {
                    (*ppf_replay_gain)[AUDIO_REPLAY_GAIN_TRACK] = us_atof( psz_val );
                    free( psz_val );
                }
            }
            else if ( !strncasecmp(psz_comment, "REPLAYGAIN_ALBUM_GAIN=", 22) )
            {
                psz_val = malloc( strlen(p+1) + 1 );
                if (!psz_val) continue;
                if( sscanf( ++p, "%s dB", psz_val ) == 1 )
                {
                    (*ppf_replay_gain)[AUDIO_REPLAY_GAIN_ALBUM] = us_atof( psz_val );
                    free( psz_val );
                }
            }
            else if ( !strncasecmp(psz_comment, "REPLAYGAIN_ALBUM_PEAK=", 22) )
            {
                (*ppf_replay_peak)[AUDIO_REPLAY_GAIN_ALBUM] = us_atof( ++p );
            }
            else if ( !strncasecmp(psz_comment, "REPLAYGAIN_TRACK_PEAK=", 22) )
            {
                (*ppf_replay_peak)[AUDIO_REPLAY_GAIN_TRACK] = us_atof( ++p );
            }
        }
        else if( !strncasecmp(psz_comment, "CHAPTER", 7) )
        {
            unsigned int i_chapt;
            seekpoint_t *p_seekpoint = NULL;
            for( int i = 0; psz_comment[i] && psz_comment[i] != '='; i++ )
                if( psz_comment[i] >= 'a' && psz_comment[i] <= 'z' )
                    psz_comment[i] -= 'a' - 'A';
            if( strstr( psz_comment, "NAME=" ) &&
                    sscanf( psz_comment, "CHAPTER%uNAME=", &i_chapt ) == 1 )
            {
                char *p = strchr( psz_comment, '=' );
                p_seekpoint = getChapterEntry( i_chapt, &chapters_array );
                if ( !p || ! p_seekpoint ) goto next_comment;
                if ( ! p_seekpoint->psz_name )
                    p_seekpoint->psz_name = strdup( ++p );
            }
            else if( sscanf( psz_comment, "CHAPTER%u=", &i_chapt ) == 1 )
            {
                unsigned int h, m, s, ms;
                char *p = strchr( psz_comment, '=' );
                if( p && sscanf( ++p, "%u:%u:%u.%u", &h, &m, &s, &ms ) == 4 )
                {
                    p_seekpoint = getChapterEntry( i_chapt, &chapters_array );
                    if ( ! p_seekpoint ) goto next_comment;
                    p_seekpoint->i_time_offset =
                      (((int64_t)h * 3600 + (int64_t)m * 60 + (int64_t)s) * 1000 + ms) * 1000;
                }
            }
        }
        else if( strchr( psz_comment, '=' ) )
        {
            /* generic (PERFORMER/LICENSE/ORGANIZATION/LOCATION/CONTACT/ISRC,
             * undocumented tags and replay gain ) */
            char *p = strchr( psz_comment, '=' );
            *p++ = '\0';
            for( int i = 0; psz_comment[i]; i++ )
                if( psz_comment[i] >= 'a' && psz_comment[i] <= 'z' )
                    psz_comment[i] -= 'a' - 'A';
            vlc_meta_AddExtra( p_meta, psz_comment, p );
        }
#undef IF_EXTRACT
next_comment:
        free( psz_comment );
        RM( comment_size );
    }
#undef RM
    if( i_seekpoint && ppp_seekpoint )
    {
        for ( unsigned int i=0; i<chapters_array.i_size; i++ )
        {
            if ( !chapters_array.pp_chapters[i] ) continue;
            TAB_APPEND_CAST( (seekpoint_t**), *i_seekpoint, *ppp_seekpoint,
                chapters_array.pp_chapters[i] );
        }
    }
    free( chapters_array.pp_chapters );
}
