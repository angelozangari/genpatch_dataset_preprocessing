}
void vlc_tdestroy (void *root, void (*freenode) (void *))
{
    const void **tab;
    size_t count;
    assert (freenode != NULL);
    /* Enumerate nodes in order */
    vlc_mutex_lock (&list.lock);
    assert (list.count == 0);
    twalk (root, list_nodes);
    tab = list.tab;
    count = list.count;
    list.tab = NULL;
    list.count = 0;
    vlc_mutex_unlock (&list.lock);
    /* Destroy the tree */
    vlc_mutex_lock (&smallest.lock);
    for (size_t i = 0; i < count; i++)
    {
         void *node  = tab[i];
         smallest.node = node;
         node = tdelete (node, &root, cmp_smallest);
         assert (node != NULL);
    }
    vlc_mutex_unlock (&smallest.lock);
    assert (root == NULL);
    /* Destroy the nodes */
    for (size_t i = 0; i < count; i++)
         freenode ((void *)(tab[i]));
    free (tab);
}
