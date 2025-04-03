 *****************************************************************************/
void input_ControlVarNavigation( input_thread_t *p_input )
{
    vlc_value_t val, text;
    int  i;
    /* Create more command variables */
    if( p_input->p->i_title > 1 )
    {
        var_Create( p_input, "next-title", VLC_VAR_VOID );
        text.psz_string = _("Next title");
        var_Change( p_input, "next-title", VLC_VAR_SETTEXT, &text, NULL );
        var_AddCallback( p_input, "next-title", TitleCallback, NULL );
        var_Create( p_input, "prev-title", VLC_VAR_VOID );
        text.psz_string = _("Previous title");
        var_Change( p_input, "prev-title", VLC_VAR_SETTEXT, &text, NULL );
        var_AddCallback( p_input, "prev-title", TitleCallback, NULL );
    }
    /* Create title and navigation */
    val.psz_string = malloc( sizeof("title ") + 5 );
    if( !val.psz_string )
        return;
    var_Change( p_input, "title", VLC_VAR_CLEARCHOICES, NULL, NULL );
    for( i = 0; i < p_input->p->i_title; i++ )
    {
        vlc_value_t val2, text2;
        int j;
        /* Add Navigation entries */
        sprintf( val.psz_string,  "title %2i", i );
        var_Destroy( p_input, val.psz_string );
        var_Create( p_input, val.psz_string,
                    VLC_VAR_INTEGER|VLC_VAR_HASCHOICE|VLC_VAR_ISCOMMAND );
        var_AddCallback( p_input, val.psz_string,
                         NavigationCallback, (void *)(intptr_t)i );
        char psz_length[MSTRTIME_MAX_SIZE + sizeof(" []")] = "";
        if( p_input->p->title[i]->i_length > 0 )
        {
            strcpy( psz_length, " [" );
            secstotimestr( &psz_length[2], p_input->p->title[i]->i_length / CLOCK_FREQ );
            strcat( psz_length, "]" );
        }
        if( p_input->p->title[i]->psz_name == NULL ||
            *p_input->p->title[i]->psz_name == '\0' )
        {
            if( asprintf( &text.psz_string, _("Title %i%s"),
                          i + p_input->p->i_title_offset, psz_length ) == -1 )
                continue;
        }
        else
        {
            if( asprintf( &text.psz_string, "%s%s",
                          p_input->p->title[i]->psz_name, psz_length ) == -1 )
                continue;
        }
        var_Change( p_input, "navigation", VLC_VAR_ADDCHOICE, &val, &text );
        /* Add title choice */
        val2.i_int = i;
        var_Change( p_input, "title", VLC_VAR_ADDCHOICE, &val2, &text );
        free( text.psz_string );
        for( j = 0; j < p_input->p->title[i]->i_seekpoint; j++ )
        {
            val2.i_int = j;
            if( p_input->p->title[i]->seekpoint[j]->psz_name == NULL ||
                *p_input->p->title[i]->seekpoint[j]->psz_name == '\0' )
            {
                /* Default value */
                if( asprintf( &text2.psz_string, _("Chapter %i"),
                          j + p_input->p->i_seekpoint_offset ) == -1 )
                    continue;
            }
            else
            {
                text2.psz_string =
                    strdup( p_input->p->title[i]->seekpoint[j]->psz_name );
            }
            var_Change( p_input, val.psz_string, VLC_VAR_ADDCHOICE,
                        &val2, &text2 );
            free( text2.psz_string );
        }
    }
    free( val.psz_string );
}
