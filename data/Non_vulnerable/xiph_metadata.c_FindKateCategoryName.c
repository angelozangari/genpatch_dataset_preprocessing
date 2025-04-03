}
const char *FindKateCategoryName( const char *psz_tag )
{
    for( size_t i = 0; i < sizeof(Katei18nCategories)/sizeof(Katei18nCategories[0]); i++ )
    {
        if( !strcmp( psz_tag, Katei18nCategories[i].psz_tag ) )
            return Katei18nCategories[i].psz_i18n;
    }
    return N_("Unknown category");
}
