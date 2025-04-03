}
static int hexdigit (char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 0xA;
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 0xa;
    return -1;
}
