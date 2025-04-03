}
v4l2_std_id var_InheritStandard (vlc_object_t *obj, const char *varname)
{
    char *name = var_InheritString (obj, varname);
    if (name == NULL)
        return V4L2_STD_UNKNOWN;
    const size_t n = sizeof (standards_vlc) / sizeof (*standards_vlc);
    static_assert (sizeof (standards_vlc) / sizeof (*standards_vlc)
                         == sizeof (standards_v4l2) / sizeof (*standards_v4l2),
                   "Inconsistent standards tables");
    static_assert (sizeof (standards_vlc) / sizeof (*standards_vlc)
                         == sizeof (standards_user) / sizeof (*standards_user),
                   "Inconsistent standards tables");
    for (size_t i = 0; i < n; i++)
        if (strcasecmp (name, standards_vlc[i]) == 0)
        {
            free (name);
            return standards_v4l2[i];
        }
    /* Backward compatibility with old versions using V4L2 magic numbers */
    char *end;
    v4l2_std_id std = strtoull (name, &end, 0);
    if (*end != '\0')
    {
        msg_Err (obj, "unknown video standard \"%s\"", name);
        std = V4L2_STD_UNKNOWN;
    }
    free (name);
    return std;
}
