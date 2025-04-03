 *****************************************************************************/
void input_ControlVarTitle( input_thread_t *p_input, int i_title )
{
    input_title_t *t = p_input->p->title[i_title];
    vlc_value_t text;
    int  i;
    /* Create/Destroy command variables */
    if( t->i_seekpoint <= 1 )
    {
        var_Destroy( p_input, "next-chapter" );
        var_Destroy( p_input, "prev-chapter" );
    }
    else if( var_Type( p_input, "next-chapter" ) == 0 )
    {
        var_Create( p_input, "next-chapter", VLC_VAR_VOID );
        text.psz_string = _("Next chapter");
        var_Change( p_input, "next-chapter", VLC_VAR_SETTEXT, &text, NULL );
        var_AddCallback( p_input, "next-chapter", SeekpointCallback, NULL );
        var_Create( p_input, "prev-chapter", VLC_VAR_VOID );
        text.psz_string = _("Previous chapter");
        var_Change( p_input, "prev-chapter", VLC_VAR_SETTEXT, &text, NULL );
        var_AddCallback( p_input, "prev-chapter", SeekpointCallback, NULL );
    }
    /* Build chapter list */
    var_Change( p_input, "chapter", VLC_VAR_CLEARCHOICES, NULL, NULL );
    for( i = 0; i <  t->i_seekpoint; i++ )
    {
        vlc_value_t val;
        val.i_int = i;
        if( t->seekpoint[i]->psz_name == NULL ||
            *t->seekpoint[i]->psz_name == '\0' )
        {
            /* Default value */
            if( asprintf( &text.psz_string, _("Chapter %i"),
                      i + p_input->p->i_seekpoint_offset ) == -1 )
                continue;
        }
        else
        {
            text.psz_string = strdup( t->seekpoint[i]->psz_name );
        }
        var_Change( p_input, "chapter", VLC_VAR_ADDCHOICE, &val, &text );
        free( text.psz_string );
    }
}
