 */
char **subtitles_Detect( input_thread_t *p_this, char *psz_path,
                         const char *psz_name_org )
{
    int i_fuzzy = var_GetInteger( p_this, "sub-autodetect-fuzzy" );
    if ( i_fuzzy == 0 )
        return NULL;
    int j, i_result2, i_sub_count, i_fname_len;
    char *f_fname_noext = NULL, *f_fname_trim = NULL;
    char **subdirs; /* list of subdirectories to look in */
    vlc_subfn_t *result = NULL; /* unsorted results */
    char **result2; /* sorted results */
    if( !psz_name_org )
        return NULL;
    char *psz_fname = make_path( psz_name_org );
    if( !psz_fname )
        return NULL;
    /* extract filename & dirname from psz_fname */
    char *f_dir = strdup( psz_fname );
    if( f_dir == NULL )
    {
        free( psz_fname );
        return NULL;
    }
    const char *f_fname = strrchr( psz_fname, DIR_SEP_CHAR );
    if( !f_fname )
    {
        free( f_dir );
        free( psz_fname );
        return NULL;
    }
    f_fname++; /* Skip the '/' */
    f_dir[f_fname - psz_fname] = 0; /* keep dir separator in f_dir */
    i_fname_len = strlen( f_fname );
    f_fname_noext = malloc(i_fname_len + 1);
    f_fname_trim = malloc(i_fname_len + 1 );
    if( !f_fname_noext || !f_fname_trim )
    {
        free( f_dir );
        free( f_fname_noext );
        free( f_fname_trim );
        free( psz_fname );
        return NULL;
    }
    strcpy_strip_ext( f_fname_noext, f_fname );
    strcpy_trim( f_fname_trim, f_fname_noext );
    result = calloc( MAX_SUBTITLE_FILES+1, sizeof(vlc_subfn_t) ); /* We check it later (simplify code) */
    subdirs = paths_to_list( f_dir, psz_path );
    for( j = -1, i_sub_count = 0; (j == -1) || ( j >= 0 && subdirs != NULL && subdirs[j] != NULL ); j++ )
    {
        const char *psz_dir = (j < 0) ? f_dir : subdirs[j];
        if( psz_dir == NULL || ( j >= 0 && !strcmp( psz_dir, f_dir ) ) )
            continue;
        /* parse psz_src dir */
        DIR *dir = vlc_opendir( psz_dir );
        if( dir == NULL )
            continue;
        msg_Dbg( p_this, "looking for a subtitle file in %s", psz_dir );
        const char *psz_name;
        while( (psz_name = vlc_readdir( dir )) && i_sub_count < MAX_SUBTITLE_FILES )
        {
            if( psz_name[0] == '.' || !subtitles_Filter( psz_name ) )
                continue;
            char tmp_fname_noext[strlen( psz_name ) + 1];
            char tmp_fname_trim[strlen( psz_name ) + 1];
            char tmp_fname_ext[strlen( psz_name ) + 1];
            const char *tmp;
            int i_prio = SUB_PRIORITY_NONE;
            /* retrieve various parts of the filename */
            strcpy_strip_ext( tmp_fname_noext, psz_name );
            strcpy_get_ext( tmp_fname_ext, psz_name );
            strcpy_trim( tmp_fname_trim, tmp_fname_noext );
            if( !strcmp( tmp_fname_trim, f_fname_trim ) )
            {
                /* matches the movie name exactly */
                i_prio = SUB_PRIORITY_MATCH_ALL;
            }
            else if( (tmp = strstr( tmp_fname_trim, f_fname_trim )) )
            {
                /* contains the movie name */
                tmp += strlen( f_fname_trim );
                if( whiteonly( tmp ) )
                {
                    /* chars in front of the movie name */
                    i_prio = SUB_PRIORITY_MATCH_RIGHT;
                }
                else
                {
                    /* chars after (and possibly in front of)
                     * the movie name */
                    i_prio = SUB_PRIORITY_MATCH_LEFT;
                }
            }
            else if( j == -1 )
            {
                /* doesn't contain the movie name, prefer files in f_dir over subdirs */
                i_prio = SUB_PRIORITY_MATCH_NONE;
            }
            if( i_prio >= i_fuzzy )
            {
                char psz_path[strlen( psz_dir ) + strlen( psz_name ) + 2];
                struct stat st;
                sprintf( psz_path, "%s"DIR_SEP"%s", psz_dir, psz_name );
                if( !strcmp( psz_path, psz_fname ) )
                    continue;
                if( !vlc_stat( psz_path, &st ) && S_ISREG( st.st_mode ) && result )
                {
                    msg_Dbg( p_this,
                            "autodetected subtitle: %s with priority %d",
                            psz_path, i_prio );
                    result[i_sub_count].priority = i_prio;
                    result[i_sub_count].psz_fname = strdup( psz_path );
                    result[i_sub_count].psz_ext = strdup(tmp_fname_ext);
                    i_sub_count++;
                }
                else
                {
                    msg_Dbg( p_this, "stat failed (autodetecting subtitle: %s with priority %d)",
                             psz_path, i_prio );
                }
            }
        }
        closedir( dir );
    }
    if( subdirs )
    {
        for( j = 0; subdirs[j]; j++ )
            free( subdirs[j] );
        free( subdirs );
    }
    free( f_dir );
    free( f_fname_trim );
    free( f_fname_noext );
    free( psz_fname );
    if( !result )
        return NULL;
    qsort( result, i_sub_count, sizeof(vlc_subfn_t), compare_sub_priority );
    result2 = calloc( i_sub_count + 1, sizeof(char*) );
    for( j = 0, i_result2 = 0; j < i_sub_count && result2 != NULL; j++ )
    {
        bool b_reject = false;
        if( !result[j].psz_fname || !result[j].psz_ext ) /* memory out */
            break;
        if( !strcasecmp( result[j].psz_ext, "sub" ) )
        {
            int i;
            for( i = 0; i < i_sub_count; i++ )
            {
                if( result[i].psz_fname && result[i].psz_ext &&
                    !strncasecmp( result[j].psz_fname, result[i].psz_fname,
                                  strlen( result[j].psz_fname) - 3 ) &&
                    !strcasecmp( result[i].psz_ext, "idx" ) )
                    break;
            }
            if( i < i_sub_count )
                b_reject = true;
        }
        else if( !strcasecmp( result[j].psz_ext, "cdg" ) )
        {
            if( result[j].priority < SUB_PRIORITY_MATCH_ALL )
                b_reject = true;
        }
        /* */
        if( !b_reject )
            result2[i_result2++] = strdup( result[j].psz_fname );
    }
    for( j = 0; j < i_sub_count; j++ )
    {
        free( result[j].psz_fname );
        free( result[j].psz_ext );
    }
    free( result );
    return result2;
}
