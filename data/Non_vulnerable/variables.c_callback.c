}
static int callback( vlc_object_t* p_this, char const *psz_var,
                     vlc_value_t oldval, vlc_value_t newval, void *p_data)
{
    (void)p_this;    (void)oldval;
    int i;
    // Check the parameters
    assert( p_data == psz_var_name );
    // Find the variable
    for( i = 0; i < i_var_count; i++ )
    {
        if( !strcmp( psz_var_name[i], psz_var ) )
            break;
    }
    // Check the variable is known
    assert( i < i_var_count );
    var_value[i].i_int = newval.i_int;
    return VLC_SUCCESS;
}
