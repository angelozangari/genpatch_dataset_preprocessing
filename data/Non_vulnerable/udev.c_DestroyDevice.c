}
static void DestroyDevice (void *data)
{
    struct device *d = data;
    if (d->sd)
        services_discovery_RemoveItem (d->sd, d->item);
    vlc_gc_decref (d->item);
    free (d);
}
