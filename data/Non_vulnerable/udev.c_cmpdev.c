 */
static int cmpdev (const void *a, const void *b)
{
    const dev_t *da = a, *db = b;
    dev_t delta = *da - *db;
    if (sizeof (delta) > sizeof (int))
        return delta ? (((signed)delta > 0) ? 1 : -1) : 0;
    return (signed)delta;
}
