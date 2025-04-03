 */
char *decode_URI_duplicate (const char *str)
{
    char *buf = strdup (str);
    if (decode_URI (buf) == NULL)
    {
        free (buf);
        buf = NULL;
    }
    return buf;
}
