}
static void DelApp (void *data)
{
    struct app *app = data;
    services_discovery_RemoveItem (app->owner, app->item);
    vlc_gc_decref (app->item);
    free (app);
}
