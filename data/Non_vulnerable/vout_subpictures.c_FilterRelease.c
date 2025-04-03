};
static void FilterRelease(filter_t *filter)
{
    if (filter->p_module)
        module_unneed(filter, filter->p_module);
    if (filter->p_owner)
        free(filter->p_owner);
    vlc_object_release(filter);
}
