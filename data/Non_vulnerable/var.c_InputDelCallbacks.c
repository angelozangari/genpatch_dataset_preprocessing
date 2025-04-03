}
static void InputDelCallbacks( input_thread_t *p_input,
                               const vlc_input_callback_t *p_callbacks )
{
    int i;
    for( i = 0; p_callbacks[i].psz_name != NULL; i++ )
        var_DelCallback( p_input,
                         p_callbacks[i].psz_name,
                         p_callbacks[i].callback, NULL );
}
