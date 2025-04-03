};
static void AddCustomRatios( vout_thread_t *p_vout, const char *psz_var,
                             char *psz_list )
{
    assert( psz_list );
    char *psz_cur = psz_list;
    char *psz_next;
    while( psz_cur && *psz_cur )
    {
        vlc_value_t val, text;
        psz_next = strchr( psz_cur, ',' );
        if( psz_next )
        {
            *psz_next = '\0';
            psz_next++;
        }
        val.psz_string = psz_cur;
        text.psz_string = psz_cur;
        var_Change( p_vout, psz_var, VLC_VAR_ADDCHOICE, &val, &text);
        psz_cur = psz_next;
    }
}
