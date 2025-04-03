 */
static int Open (vlc_object_t *obj)
{
    services_discovery_t *sd = (services_discovery_t *)obj;
    LONG drives = GetLogicalDrives ();
    char mrl[12] = "file:///A:/", name[3] = "A:";
    TCHAR path[4] = TEXT("A:\\");
    for (char d = 0; d < 26; d++)
    {
        input_item_t *item;
        char letter = 'A' + d;
        /* Does this drive actually exist? */
        if (!(drives & (1 << d)))
            continue;
        /* Is it a disc drive? */
        path[0] = letter;
        if (GetDriveType (path) != DRIVE_CDROM)
            continue;
        mrl[8] = name[0] = letter;
        item = input_item_NewWithType (mrl, name,
                                       0, NULL, 0, -1, ITEM_TYPE_DISC);
        msg_Dbg (sd, "adding %s (%s)", mrl, name);
        if (item == NULL)
            break;
        services_discovery_AddItem (sd, item, _("Local drives"));
    }
    return VLC_SUCCESS;
}
