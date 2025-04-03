}
static void test_choices( libvlc_int_t *p_libvlc )
{
    vlc_value_t val, val2;
    var_Create( p_libvlc, "bla", VLC_VAR_INTEGER | VLC_VAR_HASCHOICE |
                                 VLC_VAR_ISCOMMAND );
    val.i_int = 1;
    val2.psz_string = (char*)"one";
    var_Change( p_libvlc, "bla", VLC_VAR_ADDCHOICE, &val, &val2 );
    val.i_int = 2;
    val2.psz_string = (char*)"two";
    var_Change( p_libvlc, "bla", VLC_VAR_ADDCHOICE, &val, &val2 );
    assert( var_CountChoices( p_libvlc, "bla" ) == 2 );
    var_Change( p_libvlc, "bla", VLC_VAR_DELCHOICE, &val, &val2 );
    assert( var_CountChoices( p_libvlc, "bla" ) == 1 );
    var_Change( p_libvlc, "bla", VLC_VAR_GETCHOICES, &val, &val2 );
    assert( val.p_list->i_count == 1 && val.p_list->p_values[0].i_int == 1 &&
            val2.p_list->i_count == 1 &&
            !strcmp( val2.p_list->p_values[0].psz_string, "one" ) );
    var_FreeList( &val, &val2 );
    var_Change( p_libvlc, "bla", VLC_VAR_CLEARCHOICES, NULL, NULL );
    assert( var_CountChoices( p_libvlc, "bla" ) == 0 );
    var_Destroy( p_libvlc, "bla" );
}
