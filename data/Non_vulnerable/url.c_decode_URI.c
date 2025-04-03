 */
char *decode_URI (char *str)
{
    char *in = str, *out = str;
    if (in == NULL)
        return NULL;
    char c;
    while ((c = *(in++)) != '\0')
    {
        if (c == '%')
        {
            char hex[3];
            if (!(hex[0] = *(in++)) || !(hex[1] = *(in++)))
                return NULL;
            hex[2] = '\0';
            *(out++) = strtoul (hex, NULL, 0x10);
        }
        else
            *(out++) = c;
    }
    *out = '\0';
    return str;
}
