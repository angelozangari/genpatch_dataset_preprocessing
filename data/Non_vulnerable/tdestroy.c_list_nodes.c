} list = { NULL, 0, VLC_STATIC_MUTEX };
static void list_nodes (const void *node, const VISIT which, const int depth)
{
    (void) depth;
    if (which != postorder && which != leaf)
        return;
    const void **tab = realloc (list.tab, sizeof (*tab) * (list.count + 1));
    if (unlikely(tab == NULL))
        abort ();
    tab[list.count] = *(const void **)node;
    list.tab = tab;
    list.count++;
}
