};
static int ParseInteger(uint8_t *data, size_t size)
{
    char tmp[16];
    assert(size < sizeof(tmp));
    memcpy(tmp, data, size);
    tmp[size] = '\0';
    return strtol(tmp, NULL, 10);
}
