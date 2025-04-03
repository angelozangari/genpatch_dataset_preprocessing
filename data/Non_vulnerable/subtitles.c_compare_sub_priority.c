} vlc_subfn_t;
static int compare_sub_priority( const void *a, const void *b )
{
    const vlc_subfn_t *p0 = a;
    const vlc_subfn_t *p1 = b;
    if( p0->priority > p1->priority )
        return -1;
    if( p0->priority < p1->priority )
        return 1;
#ifdef HAVE_STRCOLL
    return strcoll( p0->psz_fname, p1->psz_fname);
#else
    return strcmp( p0->psz_fname, p1->psz_fname);
#endif
}
