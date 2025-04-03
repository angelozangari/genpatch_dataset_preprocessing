}
char *str_format_meta(input_thread_t *input, const char *s)
{
    char *str;
    size_t len;
#ifdef HAVE_OPEN_MEMSTREAM
    FILE *stream = open_memstream(&str, &len);
#elif defined( _WIN32 )
    FILE *stream = vlc_win32_tmpfile();
#else
    FILE *stream = tmpfile();
#endif
    if (stream == NULL)
        return NULL;
    input_item_t *item = (input != NULL) ? input_GetItem(input) : NULL;
    char c;
    bool b_is_format = false;
    bool b_empty_if_na = false;
    assert(s != NULL);
    while ((c = *s) != '\0')
    {
        s++;
        if (!b_is_format)
        {
            if (c == '$')
            {
                b_is_format = true;
                b_empty_if_na = false;
                continue;
            }
            fputc(c, stream);
            continue;
        }
        b_is_format = false;
        switch (c)
        {
            case 'a':
                write_meta(stream, item, vlc_meta_Artist);
                break;
            case 'b':
                write_meta(stream, item, vlc_meta_Album);
                break;
            case 'c':
                write_meta(stream, item, vlc_meta_Copyright);
                break;
            case 'd':
                write_meta(stream, item, vlc_meta_Description);
                break;
            case 'e':
                write_meta(stream, item, vlc_meta_EncodedBy);
                break;
            case 'f':
                if (item != NULL && item->p_stats != NULL)
                {
                    vlc_mutex_lock(&item->p_stats->lock);
                    fprintf(stream, "%"PRIi64,
                            item->p_stats->i_displayed_pictures);
                    vlc_mutex_unlock(&item->p_stats->lock);
                }
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            case 'g':
                write_meta(stream, item, vlc_meta_Genre);
                break;
            case 'l':
                write_meta(stream, item, vlc_meta_Language);
                break;
            case 'n':
                write_meta(stream, item, vlc_meta_TrackNumber);
                break;
            case 'p':
                if (item == NULL)
                    break;
                {
                    char *value = input_item_GetNowPlayingFb(item);
                    if (value == NULL)
                        break;
                    fputs(value, stream);
                    free(value);
                }
                break;
            case 'r':
                write_meta(stream, item, vlc_meta_Rating);
                break;
            case 's':
            {
                char *lang = NULL;
                if (input != NULL)
                    lang = var_GetNonEmptyString(input, "sub-language");
                if (lang != NULL)
                {
                    fputs(lang, stream);
                    free(lang);
                }
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            }
            case 't':
                write_meta(stream, item, vlc_meta_Title);
                break;
            case 'u':
                write_meta(stream, item, vlc_meta_URL);
                break;
            case 'A':
                write_meta(stream, item, vlc_meta_Date);
                break;
            case 'B':
                if (input != NULL)
                    fprintf(stream, "%"PRId64,
                            var_GetInteger(input, "bit-rate") / 1000);
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            case 'C':
                if (input != NULL)
                    fprintf(stream, "%"PRId64,
                            var_GetInteger(input, "chapter"));
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            case 'D':
                if (item != NULL)
                    write_duration(stream, input_item_GetDuration(item));
                else if (!b_empty_if_na)
                    fputs("--:--:--", stream);
                break;
            case 'F':
                if (item != NULL)
                {
                    char *uri = input_item_GetURI(item);
                    if (uri != NULL)
                    {
                        fputs(uri, stream);
                        free(uri);
                    }
                }
                break;
            case 'I':
                if (input != NULL)
                    fprintf(stream, "%"PRId64, var_GetInteger(input, "title"));
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            case 'L':
                if (item != NULL)
                {
                    assert(input != NULL);
                    write_duration(stream, input_item_GetDuration(item)
                                   - var_GetTime(input, "time"));
                }
                else if (!b_empty_if_na)
                    fputs("--:--:--", stream);
                break;
            case 'N':
                if (item != NULL)
                {
                    char *name = input_item_GetName(item);
                    if (name != NULL)
                    {
                        fputs(name, stream);
                        free(name);
                    }
                }
                break;
            case 'O':
            {
                char *lang = NULL;
                if (input != NULL)
                    lang = var_GetNonEmptyString(input, "audio-language");
                if (lang != NULL)
                {
                    fputs(lang, stream);
                    free(lang);
                }
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            }
            case 'P':
                if (input != NULL)
                    fprintf(stream, "%2.1f",
                            var_GetFloat(input, "position") * 100.f);
                else if (!b_empty_if_na)
                    fputs("--.-%", stream);
                break;
            case 'R':
                if (input != NULL)
                    fprintf(stream, "%.3f", var_GetFloat(input, "rate"));
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            case 'S':
                if (input != NULL)
                {
                    int rate = var_GetInteger(input, "sample-rate");
                    div_t dr = div((rate + 50) / 100, 10);
                    fprintf(stream, "%d.%01d", dr.quot, dr.rem);
                }
                else if (!b_empty_if_na)
                    fputc('-', stream);
                break;
            case 'T':
                if (input != NULL)
                    write_duration(stream, var_GetTime(input, "time"));
                else if (!b_empty_if_na)
                    fputs("--:--:--", stream);
                break;
            case 'U':
                write_meta(stream, item, vlc_meta_Publisher);
                break;
            case 'V':
            {
                float vol = 0.f;
                if (input != NULL)
                {
                    audio_output_t *aout = input_GetAout(input);
                    if (aout != NULL)
                    {
                        vol = aout_VolumeGet(aout);
                        vlc_object_release(aout);
                    }
                }
                if (vol >= 0.f)
                    fprintf(stream, "%ld", lroundf(vol * 256.f));
                else if (!b_empty_if_na)
                    fputs("---", stream);
                break;
            }
            case '_':
                fputc('\n', stream);
                break;
            case 'Z':
                if (item == NULL)
                    break;
                {
                    char *value = input_item_GetNowPlayingFb(item);
                    if (value != NULL)
                    {
                        fputs(value, stream);
                        free(value);
                    }
                    else
                    {
                        char *title = input_item_GetTitleFbName(item);
                        if (write_meta(stream, item, vlc_meta_Artist) >= 0
                            && title != NULL)
                            fputs(" - ", stream);
                        if (title != NULL)
                        {
                            fputs(title, stream);
                            free(title);
                        }
                    }
                }
                break;
            case ' ':
                b_empty_if_na = true;
                b_is_format = true;
                break;
            default:
                fputc(c, stream);
                break;
        }
    }
#ifdef HAVE_OPEN_MEMSTREAM
    return (fclose(stream) == 0) ? str : NULL;
#else
    len = ftell(stream);
    if (len != (size_t)-1)
    {
        rewind(stream);
        str = xmalloc(len + 1);
        fread(str, len, 1, stream);
        str[len] = '\0';
    }
    else
        str = NULL;
    fclose(stream);
    return str;
#endif
}
