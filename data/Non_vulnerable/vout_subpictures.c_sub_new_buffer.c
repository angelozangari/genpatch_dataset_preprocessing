 *****************************************************************************/
static subpicture_t *sub_new_buffer(filter_t *filter)
{
    filter_owner_sys_t *sys = filter->p_owner;
    subpicture_t *subpicture = subpicture_New(NULL);
    if (subpicture)
        subpicture->i_channel = sys->channel;
    return subpicture;
}
