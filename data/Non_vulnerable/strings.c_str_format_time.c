 */
char *str_format_time( const char *tformat )
{
    time_t curtime;
    struct tm loctime;
    if (strcmp (tformat, "") == 0)
        return strdup (""); /* corner case w.r.t. strftime() return value */
    /* Get the current time.  */
    time( &curtime );
    /* Convert it to local time representation.  */
    localtime_r( &curtime, &loctime );
    for (size_t buflen = strlen (tformat) + 32;; buflen += 32)
    {
        char *str = malloc (buflen);
        if (str == NULL)
            return NULL;
        size_t len = strftime (str, buflen, tformat, &loctime);
        if (len > 0)
        {
            char *ret = realloc (str, len + 1);
            return ret ? ret : str; /* <- this cannot fail */
        }
        free (str);
    }
    assert (0);
}
