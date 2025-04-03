/* returns a pointer to a valid string, with length 0 or 3 */
static const char *GetIso639_2LangCode(const char *lang)
{
    const iso639_lang_t *pl;
    if (strlen(lang) == 2)
    {
        pl = GetLang_1(lang);
    }
    else
    {
        pl = GetLang_2B(lang);      /* try native code first */
        if (!*pl->psz_iso639_2T)
            pl = GetLang_2T(lang);  /* else fallback to english code */
    }
    return pl->psz_iso639_2T;   /* returns the english code */
}
