}
static unsigned var_GetIfIndex (vlc_object_t *obj)
{
    char *ifname = var_InheritString (obj, "miface");
    if (ifname == NULL)
        return 0;
    unsigned ifindex = if_nametoindex (ifname);
    if (ifindex == 0)
        msg_Err (obj, "invalid multicast interface: %s", ifname);
    free (ifname);
    return ifindex;
}
