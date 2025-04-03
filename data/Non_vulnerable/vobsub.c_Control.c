 *****************************************************************************/
static int Control( demux_t *p_demux, int i_query, va_list args )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int64_t *pi64, i64;
    int i;
    double *pf, f;
    switch( i_query )
    {
        case DEMUX_GET_LENGTH:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            *pi64 = (int64_t) p_sys->i_length;
            return VLC_SUCCESS;
        case DEMUX_GET_TIME:
            pi64 = (int64_t*)va_arg( args, int64_t * );
            for( i = 0; i < p_sys->i_tracks; i++ )
            {
                bool b_selected;
                /* Check the ES is selected */
                es_out_Control( p_demux->out, ES_OUT_GET_ES_STATE,
                                p_sys->track[i].p_es, &b_selected );
                if( b_selected ) break;
            }
            if( i < p_sys->i_tracks && p_sys->track[i].i_current_subtitle < p_sys->track[i].i_subtitles )
            {
                *pi64 = p_sys->track[i].p_subtitles[p_sys->track[i].i_current_subtitle].i_start;
                return VLC_SUCCESS;
            }
            return VLC_EGENERIC;
        case DEMUX_SET_TIME:
            i64 = (int64_t)va_arg( args, int64_t );
            for( i = 0; i < p_sys->i_tracks; i++ )
            {
                p_sys->track[i].i_current_subtitle = 0;
                while( p_sys->track[i].i_current_subtitle < p_sys->track[i].i_subtitles &&
                       p_sys->track[i].p_subtitles[p_sys->track[i].i_current_subtitle].i_start < i64 )
                {
                    p_sys->track[i].i_current_subtitle++;
                }
                if( p_sys->track[i].i_current_subtitle >= p_sys->track[i].i_subtitles )
                    return VLC_EGENERIC;
            }
            return VLC_SUCCESS;
        case DEMUX_GET_POSITION:
            pf = (double*)va_arg( args, double * );
            for( i = 0; i < p_sys->i_tracks; i++ )
            {
                bool b_selected;
                /* Check the ES is selected */
                es_out_Control( p_demux->out, ES_OUT_GET_ES_STATE,
                                p_sys->track[i].p_es, &b_selected );
                if( b_selected ) break;
            }
            if( p_sys->track[i].i_current_subtitle >= p_sys->track[i].i_subtitles )
            {
                *pf = 1.0;
            }
            else if( p_sys->track[i].i_subtitles > 0 )
            {
                *pf = (double)p_sys->track[i].p_subtitles[p_sys->track[i].i_current_subtitle].i_start /
                      (double)p_sys->i_length;
            }
            else
            {
                *pf = 0.0;
            }
            return VLC_SUCCESS;
        case DEMUX_SET_POSITION:
            f = (double)va_arg( args, double );
            i64 = (int64_t) f * p_sys->i_length;
            for( i = 0; i < p_sys->i_tracks; i++ )
            {
                p_sys->track[i].i_current_subtitle = 0;
                while( p_sys->track[i].i_current_subtitle < p_sys->track[i].i_subtitles &&
                       p_sys->track[i].p_subtitles[p_sys->track[i].i_current_subtitle].i_start < i64 )
                {
                    p_sys->track[i].i_current_subtitle++;
                }
                if( p_sys->track[i].i_current_subtitle >= p_sys->track[i].i_subtitles )
                    return VLC_EGENERIC;
            }
            return VLC_SUCCESS;
        case DEMUX_SET_NEXT_DEMUX_TIME:
            p_sys->i_next_demux_date = (int64_t)va_arg( args, int64_t );
            return VLC_SUCCESS;
        case DEMUX_GET_PTS_DELAY:
        case DEMUX_GET_FPS:
        case DEMUX_GET_META:
        case DEMUX_GET_TITLE_INFO:
        case DEMUX_HAS_UNSUPPORTED_META:
        case DEMUX_GET_ATTACHMENTS:
        case DEMUX_CAN_RECORD:
            return VLC_EGENERIC;
        default:
            msg_Warn( p_demux, "unknown query in subtitle control" );
            return VLC_EGENERIC;
    }
}
