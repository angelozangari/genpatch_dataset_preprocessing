}
static int write_meta(FILE *stream, input_item_t *item, vlc_meta_type_t type)
{
    if (item == NULL)
        return EOF;
    char *value = input_item_GetMeta(item, type);
    if (value == NULL)
        return EOF;
    int ret = fputs(value, stream);
    free(value);
    return ret;
}
