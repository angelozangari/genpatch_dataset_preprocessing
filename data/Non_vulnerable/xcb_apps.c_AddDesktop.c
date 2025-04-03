/*** Whole desktop ***/
static void AddDesktop(services_discovery_t *sd)
{
    input_item_t *item;
    item = input_item_NewWithType ("screen://", _("Desktop"),
                                   0, NULL, 0, -1, ITEM_TYPE_CARD);
    if (item == NULL)
        return;
    services_discovery_AddItem (sd, item, NULL);
}
