} vlc_ipc_data_t;
static LRESULT CALLBACK WMCOPYWNDPROC(HWND hwnd, UINT uMsg,
                                      WPARAM wParam, LPARAM lParam)
{
    if( uMsg == WM_QUIT  )
    {
        PostQuitMessage( 0 );
    }
    else if( uMsg == WM_COPYDATA )
    {
        COPYDATASTRUCT *pwm_data = (COPYDATASTRUCT*)lParam;
        intf_thread_t *intf = (intf_thread_t *)(uintptr_t)
            GetWindowLongPtr( hwnd, GWLP_USERDATA );
        if( intf == NULL )
            return 0; /* XXX: is this even possible? */
        /* Add files to the playlist */
        if( pwm_data->lpData )
        {
            char **ppsz_argv;
            vlc_ipc_data_t *p_data = (vlc_ipc_data_t *)pwm_data->lpData;
            size_t i_data = 0;
            int i_argc = p_data->argc, i_opt, i_options;
            ppsz_argv = (char **)malloc( i_argc * sizeof(char *) );
            for( i_opt = 0; i_opt < i_argc; i_opt++ )
            {
                ppsz_argv[i_opt] = p_data->data + i_data + sizeof(size_t);
                i_data += sizeof(size_t) + *((size_t *)(p_data->data + i_data));
            }
            for( i_opt = 0; i_opt < i_argc; i_opt++ )
            {
                i_options = 0;
                /* Count the input options */
                while( i_opt + i_options + 1 < i_argc &&
                        *ppsz_argv[ i_opt + i_options + 1 ] == ':' )
                {
                    i_options++;
                }
#warning URI conversion must be done in calling process instead!
                /* FIXME: This breaks relative paths if calling vlc.exe is
                 * started from a different working directory. */
                char *psz_URI = NULL;
                if( strstr( ppsz_argv[i_opt], "://" ) == NULL )
                    psz_URI = vlc_path2uri( ppsz_argv[i_opt], NULL );
                playlist_AddExt( pl_Get(intf),
                        (psz_URI != NULL) ? psz_URI : ppsz_argv[i_opt],
                        NULL, PLAYLIST_APPEND |
                        ( ( i_opt || p_data->enqueue ) ? 0 : PLAYLIST_GO ),
                        PLAYLIST_END, -1,
                        i_options,
                        (char const **)( i_options ? &ppsz_argv[i_opt+1] : NULL ),
                        VLC_INPUT_OPTION_TRUSTED,
                        true, pl_Unlocked );
                i_opt += i_options;
                free( psz_URI );
            }
            free( ppsz_argv );
        }
    }
    return DefWindowProc( hwnd, uMsg, wParam, lParam );
}
