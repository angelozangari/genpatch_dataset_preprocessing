 *****************************************************************************/
static void ParseVorbisComments( decoder_t *p_dec )
{
    char *psz_name, *psz_value, *psz_comment;
    int i = 0;
    while( i < p_dec->p_sys->vc.comments )
    {
        psz_comment = strdup( p_dec->p_sys->vc.user_comments[i] );
        if( !psz_comment )
            break;
        psz_name = psz_comment;
        psz_value = strchr( psz_comment, '=' );
        /* Don't add empty values */
        if( psz_value && psz_value[1] != '\0')
        {
            *psz_value = '\0';
            psz_value++;
            if( !strcasecmp( psz_name, "REPLAYGAIN_TRACK_GAIN" ) ||
                !strcasecmp( psz_name, "RG_RADIO" ) )
            {
                audio_replay_gain_t *r = &p_dec->fmt_out.audio_replay_gain;
                r->pb_gain[AUDIO_REPLAY_GAIN_TRACK] = true;
                r->pf_gain[AUDIO_REPLAY_GAIN_TRACK] = us_atof( psz_value );
            }
            else if( !strcasecmp( psz_name, "REPLAYGAIN_TRACK_PEAK" ) ||
                     !strcasecmp( psz_name, "RG_PEAK" ) )
            {
                audio_replay_gain_t *r = &p_dec->fmt_out.audio_replay_gain;
                r->pb_peak[AUDIO_REPLAY_GAIN_TRACK] = true;
                r->pf_peak[AUDIO_REPLAY_GAIN_TRACK] = us_atof( psz_value );
            }
            else if( !strcasecmp( psz_name, "REPLAYGAIN_ALBUM_GAIN" ) ||
                     !strcasecmp( psz_name, "RG_AUDIOPHILE" ) )
            {
                audio_replay_gain_t *r = &p_dec->fmt_out.audio_replay_gain;
                r->pb_gain[AUDIO_REPLAY_GAIN_ALBUM] = true;
                r->pf_gain[AUDIO_REPLAY_GAIN_ALBUM] = us_atof( psz_value );
            }
            else if( !strcasecmp( psz_name, "REPLAYGAIN_ALBUM_PEAK" ) )
            {
                audio_replay_gain_t *r = &p_dec->fmt_out.audio_replay_gain;
                r->pb_peak[AUDIO_REPLAY_GAIN_ALBUM] = true;
                r->pf_peak[AUDIO_REPLAY_GAIN_ALBUM] = us_atof( psz_value );
            }
            else if( !strcasecmp( psz_name, "METADATA_BLOCK_PICTURE" ) )
            { /* Do nothing, for now */ }
            else
            {
                if( !p_dec->p_description )
                    p_dec->p_description = vlc_meta_New();
                if( p_dec->p_description )
                    vlc_meta_AddExtra( p_dec->p_description, psz_name, psz_value );
            }
        }
        free( psz_comment );
        i++;
    }
}
