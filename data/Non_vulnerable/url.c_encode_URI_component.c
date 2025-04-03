 */
char *encode_URI_component (const char *str)
{
    size_t len = strlen (str);
    char *ret = encode_URI_bytes (str, &len);
    if (likely(ret != NULL))
        ret[len] = '\0';
    return ret;
}
