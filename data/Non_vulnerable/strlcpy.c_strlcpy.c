 */
size_t strlcpy (char *tgt, const char *src, size_t bufsize)
{
    size_t length;
    for (length = 1; (length < bufsize) && *src; length++)
        *tgt++ = *src++;
    if (bufsize)
        *tgt = '\0';
    while (*src++)
        length++;
    return length - 1;
}
